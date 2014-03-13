<?php
/**
 * @file
 * Hooks implemented on behalf of other modules by entity_merge.
 */

/**
 * Implements hook_entity_merge_register_processes()
 */
function entity_merge_entity_merge_register_processes() {
  $processes = array();
  if (module_exists('entityreference')) {
    $processes['entityreference'] = array(
      'class' => 'EntityMergeProcessEntityReference',
      'weight' => 21, // happen after the target entity is saved.
    );
  }

  if (module_exists('og')) {
    $processes['og'] = array(
      'class' => 'EntityMergeProcessOg',
      'weight' => 99, // Last thing before deleting entities.
    );
  }

  return $processes;
}

/**
 * Implements hook_entity_merge_field_items_alter().
 */
function entity_merge_entity_merge_field_items_alter(&$items, $target_entity, $context) {
  if ($context['field']['type'] == 'taxonomy_term_reference') {
    _entity_merge_taxonomy_field_items_alter($items, $target_entity, $context);
  }

  if ($context['field']['type'] == 'entityreference') {
    _entity_merge_entityreference_field_items_alter($items, $target_entity, $context);
  }

  // List field types need duplicates reducing.
  if ($context['field']['module'] == 'list') {
    _entity_merge_list_field_items_alter($items, $target_entity, $context);
  }
}

/**
 * Make sure taxonomy_term_reference fields do not reference a tid twice.
 *
 * Implements hook_entity_merge_field_items_alter() on behalf of the taxonomy
 * module.
 */
function _entity_merge_taxonomy_field_items_alter(&$items, $target_entity, $context) {
  $has_tids = array();
  foreach ($items as $delta => $item) {
    if (in_array($item['tid'], $has_tids) || empty($item['tid'])) {
      unset($items[$delta]);
    }
    else {
      $has_tids[] = $item['tid'];
    }
  }
}

/**
 * Make sure enityreference fields do not reference the same entity twice.
 *
 * Implements hook_entity_merge_field_items_alter() on behalf of the
 * entityreference module.
 *
 * @todo: Consider blocking self references.
 */
function _entity_merge_entityreference_field_items_alter(&$items, $target_entity, $context) {
  // Entity reference fields with OG behaviors need to not make any changes.
  if (module_exists('og') && !empty($context['field']['settings']['handler_settings']['behaviors']['og_behavior']['status'])) {
    list($target_id) = entity_extract_ids($context['entity_type'], $target_entity);

    // Clear out the items.
    $items = array();

    // If our target is one of our sources, retain the existing values.
    if ($target_id) {
      list($entity1_id) = entity_extract_ids($context['entity_type'], $context['entity1']);
      list($entity2_id) = entity_extract_ids($context['entity_type'], $context['entity1']);

      if ($target_id == $entity1_id) {
        if (isset($context['entity1']->{$context['field']['field_name']}[$context['langcode']])) {
          $items = $context['entity1']->{$context['field']['field_name']}[$context['langcode']];
        }
      }
      elseif ($target_id == $entity2_id) {
        if (isset($context['entity2']->{$context['field']['field_name']}[$context['langcode']])) {
          $items = $context['entity2']->{$context['field']['field_name']}[$context['langcode']];
        }
      }
    }

    // We don't need to de-dupe when just copying.
    return;
  }

  $has_target_ids = array();
  foreach ($items as $delta => $item) {
    if (in_array($item['target_id'], $has_target_ids)
      || empty($item['target_id'])) {
      unset($items[$delta]);
    }
    else {
      $has_target_ids[] = $item['target_id'];
    }
  }
}

/**
 * Reduce duplicates in fields provided by the list module.
 *
 * Implements hook_entity_merge_field_items_alter() on behalf of the list
 * module.
 */
function _entity_merge_list_field_items_alter(&$items, $target_entity, $context) {
  $has_value = array();
  foreach ($items as $delta => $item) {
    if (in_array($item['value'], $has_value)) {
      unset($items[$delta]);
    }
    else {
      $has_value[] = $item['value'];
    }
  }
}
