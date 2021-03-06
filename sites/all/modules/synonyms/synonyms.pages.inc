<?php

/**
 * @file
 * Menu page callbacks of Synonyms module.
 */

/**
 * Page callback: Outputs JSON for taxonomy autocomplete suggestions.
 *
 * This callback outputs term name suggestions in response to Ajax requests
 * made by the synonyms autocomplete widget for taxonomy term reference
 * fields. The output is a JSON object of plain-text term suggestions,
 * keyed by the user-entered value with the completed term name appended.
 * Term names containing commas are wrapped in quotes. The search is made
 * with consideration of synonyms.
 *
 * @param string $field_name
 *   The name of the term reference field.
 * @param string $entity_type
 *   Entity type to which the supplied $field_name is attached to
 * @param string $bundle
 *   Bundle name to which the supplied $field_name is attached to
 * @param string $tags_typed
 *   (optional) A comma-separated list of term names entered in the
 *   autocomplete form element. Only the last term is used for autocompletion.
 *   Defaults to '' (an empty string).
 */
function synonyms_autocomplete($field_name, $entity_type, $bundle, $tags_typed = '') {
  // If the request has a '/' in the search text, then the menu system will have
  // split it into multiple arguments, recover the intended $tags_typed.
  $args = func_get_args();
  // Shift off the $field_name argument.
  array_shift($args);
  // Shift off the $entity_type argument.
  array_shift($args);
  // Shift off the $bundle argument.
  array_shift($args);
  $tags_typed = implode('/', $args);

  // Make sure the field exists and is a taxonomy field.
  if (!($field = field_info_field($field_name)) || $field['type'] != 'taxonomy_term_reference') {
    // Error string. The JavaScript handler will realize this is not JSON and
    // will display it as debugging information.
    print t('Taxonomy field @field_name not found.', array('@field_name' => $field_name));
    exit;
  }

  if (!($instance = field_info_instance($entity_type, $field['field_name'], $bundle))) {
    // Error string. The JavaScript handler will realize this is not JSON and
    // will display it as debugging information.
    print t('There was not found an instance of @field_name in @entity_type.', array(
      '@field_name' => $field_name,
      '@entity_type' => $entity_type,
    ));
    exit;
  }

  // How many suggestions maximum we are able to output.
  $max_suggestions = $instance['widget']['settings']['suggestion_size'];

  // Whether we are allowed to suggest more than one entry per term, shall that
  // entry be either term name itself or one of its synonyms.
  $suggest_only_unique = $instance['widget']['settings']['suggest_only_unique'];

  // The user enters a comma-separated list of tags. We only autocomplete the
  // last tag.
  $tags_typed = drupal_explode_tags($tags_typed);
  $tag_last = drupal_strtolower(array_pop($tags_typed));

  $term_matches = array();
  if ($tag_last != '') {
    // Part of the criteria for the query come from the field's own settings.
    $vocabularies = array();
    $tmp = taxonomy_vocabulary_get_names();
    foreach ($field['settings']['allowed_values'] as $tree) {
      $vocabularies[$tmp[$tree['vocabulary']]->vid] = $tree['vocabulary'];
    }
    $vocabularies = taxonomy_vocabulary_load_multiple(array_keys($vocabularies));

    // Array of found suggestions, each subarray of this array will represent
    // a single suggestion entry. The sub array must at least contain the
    // following keys:
    //   tid - tid of the suggested term
    //   name - name of the suggested term
    // And optionally, if the term is suggested based on its synonym, the sub
    // array should also include the additional key:
    //   synonym - string represetnation of the synonym based on which
    //     the suggested term was included.
    $tags_return = array();

    // Firstly getting a list of tids that match by $term->name.
    $query = db_select('taxonomy_term_data', 't');
    $query->addTag('translatable');
    $query->addTag('term_access');

    // Do not select already entered terms.
    if (!empty($tags_typed)) {
      $query->condition('t.name', $tags_typed, 'NOT IN');
    }
    // Select rows that match by term name.
    $result = $query
      ->fields('t', array('tid', 'name'))
      ->condition('t.vid', array_keys($vocabularies))
      ->condition('t.name', '%' . db_like($tag_last) . '%', 'LIKE')
      ->range(0, $max_suggestions)
      ->execute()
      ->fetchAllKeyed();

    // Converting results into another format.
    foreach ($result as $tid => $name) {
      $tags_return[] = array(
        'name' => $name,
        'tid' => $tid,
      );
    }

    $synonym_tids = array();
    // Now we go vocabulary by vocabulary looking through synonym fields.
    foreach ($vocabularies as $vocabulary) {
      // Now we go a synonym field by synonym field gathering suggestions.
      // Since officially EntityFieldQuery doesn't support OR conditions
      // we are enforced to go a field by field querying multiple times the DB.
      $bundle = field_extract_bundle('taxonomy_term', $vocabulary);
      foreach (synonyms_synonyms_fields($vocabulary) as $field) {
        $field = field_info_field($field);
        $instance = field_info_instance('taxonomy_term', $field['field_name'], $bundle);
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'taxonomy_term')
          ->entityCondition('bundle', $vocabulary->machine_name);

        // We let the class that defines this field type as a source of synonyms
        // filter out and provide its suggestions based on this field.
        $class = synonyms_extractor_info($field['type']);
        call_user_func(array($class, 'processEntityFieldQuery'), $tag_last, $query, $field, $instance);

        if (!empty($tags_typed)) {
          $query->propertyCondition('name', $tags_typed, 'NOT IN');
        }
        if ($suggest_only_unique && !empty($tags_return)) {
          $tmp = array();
          foreach ($tags_return as $tag_return) {
            $tmp[] = $tag_return['tid'];
          }
          // We don't want to search among the terms already found by term name.
          $query->entityCondition('entity_id', $tmp, 'NOT IN');
        }
        if ($suggest_only_unique && !empty($synonym_tids)) {
          // We also don't want to search among the terms already mached by
          // previous synonym fields.
          $query->entityCondition('entity_id', $synonym_tids, 'NOT IN');
        }
        $tmp = $query->execute();
        if (!empty($tmp)) {
          // Merging the results.
          $tmp = array_keys($tmp['taxonomy_term']);
          $synonym_tids = array_merge($synonym_tids, $tmp);
        }
      }
    }

    if (!empty($synonym_tids)) {
      foreach (taxonomy_term_load_multiple($synonym_tids) as $synonym_term) {
        $tmp = array(
          'name' => $synonym_term->name,
          'tid' => $synonym_term->tid,
        );
        // Additionally we have to find out which synonym triggered inclusion of
        // this term.
        $synonyms = synonyms_get_term_synonyms($synonym_term);
        foreach ($synonyms as $item) {
          if (strpos(mb_strtoupper($item['value'], 'UTF-8'), mb_strtoupper($tag_last, 'UTF-8')) !== FALSE) {
            $tags_return[] = array('synonym' => $item['value']) + $tmp;
            if ($suggest_only_unique) {
              // We just want to output 1 single suggestion entry per term, so
              // one synonym is enough.
              break;
            }
          }
        }
      }
    }
    $prefix = count($tags_typed) ? drupal_implode_tags($tags_typed) . ', ' : '';

    if (count($tags_return) > $max_suggestions) {
      $tags_return = array_slice($tags_return, 0, $max_suggestions);
    }

    // Now formatting the results.
    foreach ($tags_return as $info) {
      $n = $info['name'];
      // Term names containing commas or quotes must be wrapped in quotes.
      if (strpos($info['name'], ',') !== FALSE || strpos($info['name'], '"') !== FALSE) {
        $n = '"' . str_replace('"', '""', $info['name']) . '"';
      }

      if (isset($info['synonym'])) {
        $display_name = t('@synonym, synonym of %term', array('@synonym' => $info['synonym'], '%term' => $info['name']));
      }
      else {
        $display_name = check_plain($info['name']);
      }
      while (isset($term_matches[$prefix . $n])) {
        $n .= ' ';
      }
      $term_matches[$prefix . $n] = $display_name;
    }
  }
  drupal_json_output($term_matches);
}
