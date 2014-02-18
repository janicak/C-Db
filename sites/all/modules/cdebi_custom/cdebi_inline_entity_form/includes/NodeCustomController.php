<?php
class NodeCustomController extends NodeInlineEntityFormController {
    /**
   * Returns an array of fields (which can be either Field API fields or
   * properties defined through hook_entity_property_info()) that should be
   * used to represent a selected entity in the IEF table.
   *
   * The IEF widget can have its own fields specified in the widget settings,
   * in which case the output of this function is ignored.
   *
   * @param $bundles
   *   An array of allowed $bundles for this widget.
   *
   * @return
   *   An array of field information, keyed by field name. Allowed keys:
   *   - type: 'field' or 'property',
   *   - label: Human readable name of the field, shown to the user.
   *   - weight: The position of the field relative to other fields.
   *   - visible: Whether the field should be displayed.
   *   Special keys for type 'field':
   *   - formatter: The formatter used to display the field, or "hidden".
   *   - settings: An array passed to the formatter. If empty, defaults are used.
   *   - delta: If provided, limits the field to just the specified delta.
   */
  public function defaultFields($bundles) {
    $info = entity_get_info($this->entityType);
    $metadata = entity_get_property_info($this->entityType);
    $fields = array();
    if (!empty($info['entity keys']['label'])) {
      $label_key = $info['entity keys']['label'];
      $fields[$label_key] = array(
        'type' => 'property',
        'label' => $metadata ? $metadata['properties'][$label_key]['label'] : t('Label'),
        'visible' => TRUE,
        'weight' => 1,
      );
    }
    else {
      $id_key = $info['entity keys']['id'];
      $fields[$id_key] = array(
        'type' => 'property',
        'label' => $metadata ? $metadata['properties'][$id_key]['label'] : t('ID'),
        'visible' => TRUE,
        'weight' => 1,
      );
    }
    if (count($bundles) > 1) {
      $bundle_key = $info['entity keys']['bundle'];
      $fields[$bundle_key] = array(
        'type' => 'property',
        'label' => $metadata ? $metadata['properties'][$bundle_key]['label'] : t('Type'),
        'visible' => TRUE,
        'weight' => 2,
      );
    }
    // There, we add every single "CCK"-field the content type has
    $w = 3;
    foreach($metadata['bundles'][$bundles[0]]['properties'] as $key => $property){
        $fields[$key] = array(
            'type' => 'property',
            'label' => $property['label'],
            'visible' => TRUE,
            'weight' => $w++,
          );
    }
    return $fields;
  }
}
?>