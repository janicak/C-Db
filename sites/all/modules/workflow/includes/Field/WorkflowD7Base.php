<?php

/**
 * @file
 * Contains workflow\includes\Field\WorkflowD7Base.
 */

/*
 * A Retrofit/Stub class, that contains the most basic functions of the D8 WidgetBase class.
 * It serves as a superclass to containe the $field and $instance array structures for the Field Type and the Widget.
 * @todo D8: Remove this class.
 */
abstract class WorkflowD7Base {
  // Properties for Field and Widget.
  protected $field = array();
  protected $instance = array();
  // Properties for Field.
  protected $entity = NULL;
  protected $entity_type = '';

  /**
   * Constructor, stub for D8 WidgetBase.
   */
  public function __construct(array $field, array $instance, $entity_type = '', stdClass $entity = NULL) {
    // Properties for Widget and Field.
    $this->field = $field;
    $this->instance = $instance;
    // Properties for FieldItem.
    $this->entity = $entity;
    $this->entity_type = $entity_type;
  }

  public function getField() {
    return $this->field;
  }

  public function getInstance() {
    return $this->instance;
  }

  protected function getSettings() {
    $settings = isset($this->instance['widget']['settings']) ? $this->instance['widget']['settings'] : array();
    $field_info = self::settings();
    return $settings += $field_info['workflow']['settings'];
  }

  protected function getSetting($key) {
    if (isset($this->instance['widget']['settings'][$key])) {
      return $this->instance['widget']['settings'][$key];
    }
    else {
      $field_info = $this->settings();
      return $field_info['workflow']['settings'][$key];
    }
  }

//  /*
//   * Stub for getEntity() method on the Field and FieldItem class.
//   */
//  protected function getEntity() {
//    return $this->entity;
//  }

//  protected function setEntity(object $entity, $entity_type) {
//    $this->entity = $entity;
//    $this->entity_type = $entity_type;
//  }

  /*	
   * Saves an entity.
   *
   * Usage: call this externally if the WorkflowItem isn't called from node_save/update itself,
   * E.g., from workflow_cron().
   */
  public function entitySave($entity_type, $entity) {
    return ($entity_type == 'node') ? node_save($entity) : entity_save($entity_type, $entity);
  }

}
