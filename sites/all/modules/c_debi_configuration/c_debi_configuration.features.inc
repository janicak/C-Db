<?php
/**
 * @file
 * c_debi_configuration.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function c_debi_configuration_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
  if ($module == "ds" && $api == "ds") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function c_debi_configuration_image_default_styles() {
  $styles = array();

  // Exported image style: profile_image_page.
  $styles['profile_image_page'] = array(
    'name' => 'profile_image_page',
    'label' => 'profile_image_page',
    'effects' => array(
      1 => array(
        'label' => 'Manual Crop: Crop and scale',
        'help' => 'Crop and scale a user-selected area, respecting the ratio of the destination width and height.',
        'effect callback' => 'manualcrop_crop_and_scale_effect',
        'form callback' => 'manualcrop_crop_and_scale_form',
        'summary theme' => 'manualcrop_crop_and_scale_summary',
        'module' => 'manualcrop',
        'name' => 'manualcrop_crop_and_scale',
        'data' => array(
          'width' => 150,
          'height' => 150,
          'upscale' => 1,
          'respectminimum' => 0,
          'onlyscaleifcrop' => 0,
          'style_name' => 'profile_image_page',
        ),
        'weight' => 0,
      ),
    ),
  );

  // Exported image style: search_profile_image.
  $styles['search_profile_image'] = array(
    'name' => 'search_profile_image',
    'label' => 'Search Profile Image',
    'effects' => array(
      2 => array(
        'label' => 'Manual Crop: Automatically reuse cropped style',
        'help' => 'Load the first applied crop selection and reuse it.',
        'effect callback' => 'manualcrop_auto_reuse_effect',
        'form callback' => 'manualcrop_auto_reuse_form',
        'summary theme' => 'manualcrop_auto_reuse_summary',
        'module' => 'manualcrop',
        'name' => 'manualcrop_auto_reuse',
        'data' => array(
          'fallback_style' => 'thumbnail',
          'style_priority' => array(),
        ),
        'weight' => 0,
      ),
      3 => array(
        'label' => 'Scale',
        'help' => 'Scaling will maintain the aspect-ratio of the original image. If only a single dimension is specified, the other dimension will be calculated.',
        'effect callback' => 'image_scale_effect',
        'dimensions callback' => 'image_scale_dimensions',
        'form callback' => 'image_scale_form',
        'summary theme' => 'image_scale_summary',
        'module' => 'image',
        'name' => 'image_scale',
        'data' => array(
          'width' => 50,
          'height' => 50,
          'upscale' => 0,
        ),
        'weight' => 2,
      ),
    ),
  );

  // Exported image style: site_logo.
  $styles['site_logo'] = array(
    'name' => 'site_logo',
    'label' => 'Site Logo',
    'effects' => array(
      4 => array(
        'label' => 'Scale',
        'help' => 'Scaling will maintain the aspect-ratio of the original image. If only a single dimension is specified, the other dimension will be calculated.',
        'effect callback' => 'image_scale_effect',
        'dimensions callback' => 'image_scale_dimensions',
        'form callback' => 'image_scale_form',
        'summary theme' => 'image_scale_summary',
        'module' => 'image',
        'name' => 'image_scale',
        'data' => array(
          'width' => 30,
          'height' => 30,
          'upscale' => 0,
        ),
        'weight' => 1,
      ),
    ),
  );

  return $styles;
}

/**
 * Implements hook_name_default_formats().
 */
function c_debi_configuration_name_default_formats() {
  $formats = array();

  // Exported format: Default
  $formats['default'] = array(
    'default_format' => 'G(c|gif)',
    'sep1' => ' ',
    'sep2' => ', ',
    'sep3' => '',
    'machine_name' => 'default',
    'name' => 'Default',
  );

  // Exported format: Family
  $formats['family'] = array(
    'name' => 'Family',
    'machine_name' => 'family',
    'format' => 'f',
  );

  // Exported format: Title Family
  $formats['formal'] = array(
    'name' => 'Title Family',
    'machine_name' => 'formal',
    'format' => 't+if',
  );

  // Exported format: Full
  $formats['full'] = array(
    'name' => 'Full',
    'machine_name' => 'full',
    'format' => '((((t+ig)+im)+if)+is)+jc',
  );

  // Exported format: Given
  $formats['given'] = array(
    'name' => 'Given',
    'machine_name' => 'given',
    'format' => 'g',
  );

  // Exported format: Given Family
  $formats['short_full'] = array(
    'name' => 'Given Family',
    'machine_name' => 'short_full',
    'format' => 'g+if',
  );

  return $formats;
}

/**
 * Implements hook_node_info().
 */
function c_debi_configuration_node_info() {
  $items = array(
    'node_block' => array(
      'name' => t('Node Block'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implements hook_default_search_api_server().
 */
function c_debi_configuration_default_search_api_server() {
  $items = array();
  $items['solr'] = entity_import('search_api_server', '{
    "name" : "Solr",
    "machine_name" : "solr",
    "description" : "",
    "class" : "search_api_solr_service",
    "options" : {
      "clean_ids" : true,
      "scheme" : "http",
      "host" : "localhost",
      "port" : "8983",
      "path" : "\\/solr",
      "http_user" : "admin",
      "http_pass" : "microsoft",
      "excerpt" : 0,
      "retrieve_data" : 0,
      "highlight_data" : 0,
      "http_method" : "POST",
      "autocorrect_spell" : 1,
      "autocorrect_suggest_words" : 1
    },
    "enabled" : "1",
    "rdf_mapping" : []
  }');
  return $items;
}
