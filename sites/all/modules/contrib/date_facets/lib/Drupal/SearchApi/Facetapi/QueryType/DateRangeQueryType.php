<?php

/**
 * @file
 * Contains Drupal_SearchApi_Facetapi_QueryType_DateRangeQueryType.
 */

/**
 * Date range query type plugin for the Apache Solr Search Integration adapter.
 */
class Drupal_SearchApi_Facetapi_QueryType_DateRangeQueryType extends SearchApiFacetapiDate implements FacetapiQueryTypeInterface {

  /**
   * Implements FacetapiQueryTypeInterface::getType().
   */
  static public function getType() {
    return 'date_range';
  }

  /**
   * Implements FacetapiQueryTypeInterface::execute().
   */
  public function execute($query) {
    $this->adapter->addFacet($this->facet, $query);
    if ($active = $this->adapter->getActiveItems($this->facet)) {
      // Check the first value since only one is allowed.
      $filter = self::mapFacetItemToFilter(key($active));
      if ($filter) {
        $this->addFacetFilter($query, $this->facet['field'], $filter);
      }
    }
  }

  /**
   * Generate valid date ranges (timestamps) to be used in a SQL query.
   */
  protected function generateRange($range = array()) {
    if (empty($range)) {
      $start = REQUEST_TIME;
      $end = REQUEST_TIME + DATE_RANGE_UNIT_DAY;
    }
    else {
      // Generate the timestamp for both start and end date ranges.
      foreach (array('start', 'end') as $item) {
        // Start by initializing it to the current time.
        $$item = REQUEST_TIME;
        $unit = $range['date_range_' . $item . '_unit'];
        $amount = (int) $range['date_range_' . $item . '_amount'];
        switch ($unit) {
          case 'HOUR':
            $unit = (int) DATE_RANGE_UNIT_HOUR;
            break;
          case 'DAY':
            $unit = (int) DATE_RANGE_UNIT_DAY;
            break;
          case 'MONTH':
            $unit = (int) DATE_RANGE_UNIT_MONTH;
            break;
          case 'YEAR':
            $unit = (int) DATE_RANGE_UNIT_YEAR;
            break;
        }
        // Based on which operation, either add or subtract the appropriate
        // amount from the time.
        // In each case, the amount we subtract is the amount of the unit, times
        // the value of the unit.
        switch ($range['date_range_' . $item . '_op']) {
          case '-':
            $$item -= ($amount * $unit);
            break;
          case '+':
            $$item += ($amount * $unit);
            break;
        }
      }
      // If the ops are NOW, we set the times accordingly.
      if ($range['date_range_start_op'] == 'NOW') {
        $start = REQUEST_TIME;
      }
      if ($range['date_range_end_op'] == 'NOW') {
        $end = REQUEST_TIME;
      }
    }
    return array($start, $end);
  }

  /**
   * Implements FacetapiQueryTypeInterface::build().
   *
   * Unlike normal facets, we provide a static list of options.
   */
  public function build() {
    $facet = $this->adapter->getFacet($this->facet);
    $search_ids = drupal_static('search_api_facetapi_active_facets', array());

    if (empty($search_ids[$facet['name']]) || !search_api_current_search($search_ids[$facet['name']])) {
      return array();
    }
    $search_id = $search_ids[$facet['name']];

    $build = array();
    $search = search_api_current_search($search_id);

    $results = $search[1];
    if (!$results['result count']) {
      return array();
    }

    // Executes query, iterates over results.
    if (isset($results['search_api_facets']) && isset($results['search_api_facets'][$this->facet['field']])) {
      $values = $results['search_api_facets'][$this->facet['field']];

      $realm = facetapi_realm_load('block');
      $settings = $this->adapter->getFacetSettings($this->facet, $realm);
      $ranges = (isset($settings->settings['ranges']) && !empty($settings->settings['ranges']) ? $settings->settings['ranges'] : date_facets_default_ranges());
      // Build the markup for the facet's date ranges.
      $build = date_facets_get_ranges($ranges);


      // Calculate values by facet.
      foreach ($values as $value) {
        $value['filter'] = str_replace('"', '', $value['filter']);
        $diff = REQUEST_TIME - $value['filter'];

        foreach ($ranges as $key => $item) {
          list($start, $end) = $this->generateRange($item);
          if ($diff < ($end - $start)) {
            $build[$key]['#count'] += $value['count'];
          }
        }
      }
    }

    // Unset empty items.
    foreach ($build as $key => $item) {
      if ($item['#count'] === NULL) {
        unset($build[$key]);
      }
    }

    // Gets total number of documents matched in search.
    $total = $results['result count'];
    $keys_of_active_facets = array();
    // Gets active facets, starts building hierarchy.
    foreach ($this->adapter->getActiveItems($this->facet) as $key => $item) {
      // If the item is active, the count is the result set count.
      $build[$key]['#count'] = $total;
      $keys_of_active_facets[] = $key;
    }

    // If we have active item, unset other items.
    $settings = $facet->getSettings()->settings;
    if ((isset($settings['operator'])) && ($settings['operator'] !== FACETAPI_OPERATOR_OR)) {
      if (!empty($keys_of_active_facets)) {
        foreach ($build as $key => $item) {
          if (!in_array($key, $keys_of_active_facets)) {
            unset($build[$key]);
          }
        }
      }
    }

    return $build;
  }

  /**
   * Maps a facet item to a filter.
   *
   * @param string $key
   *   Facet item key, for example 'past_hour'.
   *
   * @return string|false
   *   A string that can be used as a filter, false if no filter was found.
   */
  public function mapFacetItemToFilter($key) {
    $options = self::getFacetItems();
    return isset($options[$key]) ? $options[$key] : FALSE;
  }

  /**
   * Gets a list of facet items and matching filters.
   *
   * @return array
   *   List of facet items and their filters.
   */
  public function getFacetItems() {
    $now = $_SERVER['REQUEST_TIME'];
    $past_hour = strtotime('-1 hour');
    $past_24_hours = strtotime('-24 hour');
    $past_week = strtotime('-1 week');
    $past_month = strtotime('-1 month');
    $past_year = strtotime('-1 year');

    $options = array(
      'past_hour'     => "[$past_hour TO $now]",
      'past_24_hours' => "[$past_24_hours TO $now]",
      'past_week'     => "[$past_week TO $now]",
      'past_month'    => "[$past_month TO $now]",
      'past_year'     => "[$past_year TO $now]",
    );

    return $options;
  }
}
