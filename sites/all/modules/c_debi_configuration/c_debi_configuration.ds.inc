<?php
/**
 * @file
 * c_debi_configuration.ds.inc
 */

/**
 * Implements hook_ds_custom_fields_info().
 */
function c_debi_configuration_ds_custom_fields_info() {
  $export = array();

  $ds_field = new stdClass();
  $ds_field->api_version = 1;
  $ds_field->field = 'view_details';
  $ds_field->label = 'View Details';
  $ds_field->field_type = 5;
  $ds_field->entities = array(
    'dataset' => 'dataset',
    'node' => 'node',
  );
  $ds_field->ui_limit = '';
  $ds_field->properties = array(
    'code' => array(
      'value' => 'View Details',
      'format' => 'plain_text',
    ),
    'use_token' => 0,
  );
  $export['view_details'] = $ds_field;

  return $export;
}
