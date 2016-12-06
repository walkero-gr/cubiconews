<?php

function _cubiconews_admin_preferences_form($form, &$form_submit){
	$form = array();
	
	// Add the content type checkboxes
	$form['cubiconews_config_fs_nodes'] = array(	
		'#type' => 'fieldset',
		'#title' => t('Enable for the nodes')
	);
	
	$contenttypes = node_type_get_types();
	$contenttypes_arr = array();
	foreach($contenttypes AS $nodetype_key => $nodetype_data) {
		if($nodetype_key != 'cubiconews_item') {
			$contenttypes_arr[$nodetype_key] = $nodetype_data->name;
		}
	}
	$nodeavailability = variable_get('cubiconews_config_selected_contenttypes', array());	
	$form['cubiconews_config_fs_nodes']['cubiconews_config_selected_contenttypes'] = array(
	  '#type' => 'checkboxes',
	  '#title' => t('Available for the following nodes'),
	  '#default_value' => $nodeavailability,
	  '#options' => $contenttypes_arr,
	  '#description' => t('Enable on which nodes the CubicoNews will be available.'),
	);
	
	
	// Add the fields list for every enabled content type
	$form['cubiconews_config_fs_node_images'] = array(
		'#type' => 'fieldset',
		'#title' => t('Node images assignment')
	);
	
		
	foreach($contenttypes_arr AS $nodetype_key => $nodetype_data) {
		$content_type = $nodetype_key;
		if(key_exists($content_type, $nodeavailability) && ($nodeavailability[$content_type])) {
			$instances = field_info_instances('node', $content_type);
			$ctype_images_fields = array();
			foreach ($instances as $name => $instance) {
				if($instance['widget']['type'] == 'image_image') {
					$ctype_images_fields[$instance['field_name']] = $instance['label'];
				}
			}
			
			$form['cubiconews_config_fs_node_images']['cubiconews_config_image_field_' . $content_type] = array(
				'#type' => 'select',
				'#title' => '<b>' . $nodetype_data . ':</b> ' . t('Use this image field'),
				'#default_value' => variable_get('cubiconews_config_image_field_' . $content_type,''),
				'#options' => $ctype_images_fields
			);
		}
	}
	
	// Add the testing email accounts textarea
	$form['cubiconews_config_test_emails'] = array(
		'#type' => 'textarea',
		'#title' => t('Emails for testing newsletter'),
		'#description' => t('Add here a list of emails seperated by comma (,).'),
		'#default_value' => variable_get('cubiconews_config_test_emails', ''),
		'#required' => true
	);
	
	$form['btn_submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
		'#name' => 'save',
	);
	
	return $form;
}

function _cubiconews_admin_preferences_form_submit ($form, &$form_state){
	$nodeavailability = $form_state['values']['cubiconews_config_selected_contenttypes'];
	foreach($nodeavailability AS $availnode_key => $availnode_data) {
		$ctype = $availnode_data;
		if($ctype && isset($form_state['values']['cubiconews_config_image_field_' . $ctype])) {
			variable_set('cubiconews_config_image_field_' . $ctype, $form_state['values']['cubiconews_config_image_field_' . $ctype]);
		}
	}
	variable_set('cubiconews_config_selected_contenttypes', $form_state['values']['cubiconews_config_selected_contenttypes']);
	variable_set('cubiconews_config_test_emails', $form_state['values']['cubiconews_config_test_emails']);
	
	drupal_set_message(t('Configuration saved succesfully.'), 'status');
}