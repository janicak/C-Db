<?php
/**
 * @file
 * c_debi_configuration.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function c_debi_configuration_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'background';
  $context->description = '';
  $context->tag = '';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '*' => '*',
        '~admin*' => '~admin*',
      ),
    ),
  );
  $context->reactions = array(
    'backstretch' => array(
      'source' => array(
        'type' => 'entity',
        'entity' => array(
          'entity_type' => 'file',
          'ids' => '13',
        ),
        'path' => 'http://drupal.local/sites/default/files/greengradient2_0.jpg',
      ),
      'image_style' => '',
      'element' => '',
      'element_other' => '',
      'duration' => '5000',
      'fade' => '0',
      'center_x' => 1,
      'center_y' => 0,
    ),
  );
  $context->condition_mode = 0;
  $export['background'] = $context;

  return $export;
}
