<?php


//drupal_add_js(drupal_get_path('theme', 'cdebi_bootstrap') . '/js/cdebi.js');


/**
 * Overrides theme_blockify_site_name(). Adds bootstrap classes to site name.
 */
function cdebi_bootstrap_theme_blockify_site_name($variables) {
  $title = drupal_get_title();
  $site_name = $variables['site_name'];

  // If there is no page title set for this page, use an h1 for the site name.
  /**$tag = ($title !== '') ? 'span' : 'h1';*/

  $link = array(
    '#theme' => 'link',
    '#path' => '<front>',
    '#text' => '<span>' . $site_name . '</span>',
    // '#prefix' => '</' . $tag . ' id="site-name">',
    '#prefix' => '</ id="site-name">',
    //'#suffix' => '</' . $tag . '>',
    '#suffix' => '</>',
    '#options' => array(
      'attributes' => array(
        'title' => t('Return to the !site_name home page', array(
          '!site_name' => $site_name,
        )),
        'rel' => 'home',
        /*'class' => 'navbar-brand'*/
      ),
      'html' => TRUE,
    ),
  );

  return render($link);
}

/**
 * Overrides bootstrap_item_list(). Adds Bootstrap clases to item lists.
 */
function cdebi_bootstrap_item_list($variables) {
  $items = $variables['items'];
  $title = $variables['title'];
  $type = $variables['type'];
  $attributes = $variables['attributes'];
  $attributes['class'][] = 'list-group';
  $output = '';

  if (isset($title)) {
    $output .= '<h3>' . $title . '</h3>';
  }

  if (!empty($items)) {
    $output .= "<$type" . drupal_attributes($attributes) . '>';
    $num_items = count($items);
    $i = 0;
    foreach ($items as $item) {
      $attributes = array();
      $children = array();
      $data = '';
      $i++;
      if (is_array($item)) {
        foreach ($item as $key => $value) {
          if ($key == 'data') {
            $data = $value;
          }
          elseif ($key == 'children') {
            $children = $value;
          }
          else {
            $attributes[$key] = $value;
          }
        }
      }
      else {
        $data = $item;
      }
      if (count($children) > 0) {
        // Render nested list.
        $data .= theme('item_list', array(
          'items' => $children,
          'title' => NULL,
          'type' => $type,
          'attributes' => $attributes,
        ));
      }
      $attributes['class'][] = 'list-group-item';
      if ($i == 1) {
        $attributes['class'][] = 'first';
      }
      if ($i == $num_items) {
        $attributes['class'][] = 'last';
      }
      $output .= '<li' . drupal_attributes($attributes) . '>' . $data . "</li>\n";
    }
    $output .= "</$type>";
  }

  return $output;
}

/**
 * Overrides theme_biblio_author_link(). This changes the author link to point to the publications
 * list with a Search API fulltext search using their last name.
 */

function cdebi_bootstrap_biblio_author_link($variables) {
  $base  = variable_get('biblio_base', 'biblio');
  $author = $variables['author'];
  $options = isset($variables['options']) ? $variables['options'] : array();

  $link_to_profile = variable_get('biblio_author_link_profile', 0);
  $link_to_profile_path = variable_get('biblio_author_link_profile_path', 'user/[user:uid]');

  $uri = drupal_parse_url(request_uri());
  $uri = array_merge($uri, $options);
  if (!isset($uri['attributes'])) {
    $uri['attributes'] = array('rel' => 'nofollow');
  }
  $path = $uri['path'];

  if (isset($author['drupal_uid']) && $author['drupal_uid'] > 0) {
    $uri['attributes'] += array('class' => array('biblio-local-author'));
  }
  if (variable_get('biblio_links_target_new_window', null)){
    $uri['attributes'] +=  array('target'=>'_blank');
    $uri['html'] = TRUE;
  }

  if ($link_to_profile && isset($author['drupal_uid'])  &&  $author['drupal_uid'] > 0) {
    $data['user'] = user_load($author['drupal_uid']);
    $path = token_replace($link_to_profile_path, $data);
    $alias = drupal_get_path_alias($path);
    $path_profile = variable_get('biblio_show_profile', '0') ? "$path/$base" : $alias;
    return l(trim($author['name']), $path_profile, $uri);
  }
  else {
    $uri['path'] = variable_get('biblio_base', 'biblio');
//    $uri['query']['f']['author'] = $author['cid'];
    $uri['query']['fulltext'] = $author['lastname'];
    return l(trim($author['name']), $uri['path'], $uri );
  }

}

/**
 * Overrides theme_facetapi_count to surround count with Bootstrap badge markup
*/
/**function cdebi_bootstrap_facetapi_count($variables) {
  return '<span class="badge">' . (int) $variables['count'] . '</span>';
} **/


/**
 * Overrides theme_facetapi_link_active
 * Returns HTML for an  active facet item.
 *
 * @param $variables
 *   An associative array containing the keys 'text', 'path', and 'options'. See
 *   the l() function for information about these variables.
 *
 * @see l()
 *
 * @ingroup themeable
 */

function cdebi_bootstrap_facetapi_link_active($variables) {

  // Sanitizes the link text if necessary.
  $sanitize = empty($variables['options']['html']);
  $link_text = ($sanitize) ? check_plain($variables['text']) : $variables['text'];

  // Theme function variables fro accessible markup.
  // @see http://drupal.org/node/1316580
  $accessible_vars = array(
    'text' => $variables['text'],
    'active' => TRUE,
  );

  // Builds link, passes through t() which gives us the ability to change the
  // position of the widget on a per-language basis.
  $replacements = array(
    '!facetapi_deactivate_widget' => theme('facetapi_deactivate_widget', $variables),
    '!facetapi_accessible_markup' => theme('facetapi_accessible_markup', $accessible_vars),
  );
  $variables['text'] = t('!facetapi_deactivate_widget !facetapi_accessible_markup', $replacements);
  $variables['options']['html'] = TRUE;
  return theme_link($variables) . '<span class="activefacet-text">' . $link_text . '</span>';
}
