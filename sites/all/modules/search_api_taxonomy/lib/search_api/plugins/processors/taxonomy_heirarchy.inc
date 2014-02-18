<?php

/**
 * Simple processor to index all parent terms of a taxonomy field.
 */
class SearchApiTaxonomyHeirarchyProcessor extends SearchApiAbstractProcessor {

  protected function getTaxonomyFields() {
    $all_fields = $this->index->getFields();
    $fields = array();
    foreach ($all_fields as $k => $field) {
      if (isset($field['entity_type']) && ($field['entity_type'] == 'taxonomy_term')) {
        $fields[$k] = $field;
      }
    }

    return $fields;
  }

  public function configurationForm() {
    $form['#attached']['css'][] = drupal_get_path('module', 'search_api') . '/search_api.admin.css';

    $fields = $this->getTaxonomyFields();
    $field_options = array();
    $default_fields = array();
    if (isset($this->options['fields'])) {
      $default_fields = drupal_map_assoc(array_keys($this->options['fields']));
    }
    foreach ($fields as $name => $field) {
      $field_options[$name] = $field['name'];
      if (!empty($default_fields[$name]) || (!isset($this->options['fields']) && $this->testField($name, $field))) {
        $default_fields[$name] = $name;
      }
    }

    $form['fields'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Fields to run on'),
      '#options' => $field_options,
      '#default_value' => $default_fields,
      '#attributes' => array('class' => array('search-api-checkboxes-list')),
    );

    return $form;
  }

  protected function processFieldValue(&$value) {
    if (is_numeric($value) && ($term = taxonomy_term_load($value))) {
      $new_value = array();
      $parents = taxonomy_get_parents_all($value);
      foreach ($parents as $parent) {
        $new_value[] = array('value' => $parent->tid, 'score' => 1);
      }
      $value = $new_value;
    }
  }
}
