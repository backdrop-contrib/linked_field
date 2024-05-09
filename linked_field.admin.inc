<?php

/**
 * @file
 * Administrative interface for Linked Field.
 */

/**
 * List all linked fields.
 *
 * @see field_ui_fields_list()
 */
function linked_field_field_list() {
  $instances = field_info_instances();
  $field_types = field_info_field_types();
  $bundles = field_info_bundles();
  $modules = system_rebuild_module_data();
  $linked_fields = array();

  $header = array(
    array('data' => t('Field name'), 'field' => 'field_name', 'sort' => 'asc'),
    array('data' => t('Linked in')),
  );
  $rows = array();
  foreach ($instances as $entity_type => $type_bundles) {
    foreach ($type_bundles as $bundle => $bundle_instances) {
      foreach ($bundle_instances as $field_name => $instance) {
        $field = field_info_field($field_name);

        // Iterate the view modes.
        foreach ($instance['display'] as $view_mode_name => $view_mode) {
          // Proceed only if the current field is linked.
          if (isset($view_mode['settings']['linked_field']) && $view_mode['settings']['linked_field']['linked']) {
            // Get some information about the field.
            $entity_info = entity_get_info($entity_type);
            $view_mode_path = ($view_mode_name != 'default') ? '/' . $view_mode_name : '';
            $admin = $entity_info['bundles'][$bundle]['admin']['real path'] . '/display' . $view_mode_path;
            $view_mode_label = ($view_mode_name != 'default') ? $entity_info['view modes'][$view_mode_name]['label'] : 'Default';
            $bundle_name = $entity_info['bundles'][$bundle]['label'];

            if (!isset($linked_fields[$field_name][$entity_type]['#label'])) {
              $linked_fields[$field_name][$entity_type]['#label'] = $entity_info['label'];
            }

            if (!isset($linked_fields[$field_name][$entity_type][$bundle]['#label'])) {
              $linked_fields[$field_name][$entity_type][$bundle]['#label'] = $bundle_name;
            }

            $linked_fields[$field_name][$entity_type][$bundle][$view_mode_name] = array(
              'admin' => $admin,
              'view_mode' => $view_mode_label,
            );
          }
        }
      }
    }
  }

  // Iterate all linked fields and build a table.
  foreach ($linked_fields as $field_name => $entity_types) {
    $rows[$field_name]['data'][0] = check_plain($field_name);
    $details = array();

    // Iterate all entity types.
    foreach ($entity_types as $entity_type => $bundles) {
      $entity_type_label = $bundles['#label'];
      $bundle_list = array();

      // Iterate all entity bundles.
      foreach (element_children($bundles) as $key) {
        $bundle_name = $bundles[$key]['#label'];

        $view_modes = $bundles[$key];

        $view_mode_list = array();

        // Iterate all bundle view modes.
        foreach (element_children($view_modes) as $key) {
          $view_mode = $view_modes[$key];
          $view_mode_list[] = l($view_mode['view_mode'], $view_mode['admin']);
        }

        $bundle_list[] = array(
          'data' => $bundle_name,
          'children' => $view_mode_list,
        );
      }

      // Add a new grouping per entity type.
      $details[] = array(
        '#type' => 'details',
        '#summary' => $entity_type_label,
        '#open' => TRUE,
        '#children' => theme('item_list', array('items' => $bundle_list)),
        '#attributes' => array('class' => array('collapsible', 'collapsed')),
      );
    }

    backdrop_add_library('system', 'backdrop.collapse');
    $rows[$field_name]['data'][1] = backdrop_render($details);
  }

  // Sort rows by field name.
  ksort($rows);
  $output = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No fields are linked yet.'),
  );

  return $output;
}
