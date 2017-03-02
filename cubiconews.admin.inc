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





function cubiconews_emails_form($form, &$form_submit) {
  $form = array();
  $output = '<h1>' . t('Manage Newsletter Recipients') . '</h1>';

  // Check first if the email already exists in our database
	$count_emails = db_select('cubiconews_emails', 'cube');
	$count_emails->fields('cube', array('status'));
	$count_emails->addExpression('COUNT(*)', 'cnt');
	$count_emails->orderBy('status', 'DESC');
	$count_emails->groupBy('status');
	//$count_emails->countQuery();
	$result = $count_emails->execute();
  $emails_num = $result->rowCount();

  if($emails_num > 0) {
  	while($record = $result->fetchAssoc()) {
  		print_r($record);
  		switch($record['status']) {
  			case 0:
  				$output .=  '<div class="disabled">' . t('Disabled emails') . ': ' . $record['cnt'] . '</div>';
  				break;
  			case 1:
  				$output .=  '<div class="enabled">' . t('Enabled emails') . ': ' . $record['cnt'] . '</div>';
  				break;
  		}
  	}
  } else {
    $output .= t('It seems you run out of recipients!');
    $output .= t('<br>If you want to import a few emails, please use the "Add Emails" form.');
  }
	//$output .=  '<a href="/cubiconews/newsletters/emails/add">' . t('Add new emails') . '</a>';

  $form['email_list_info'] = array(
    '#markup' => $output,
  );

  // $form['btn_submit'] = array(
  //   '#type' => 'submit',
  //   '#value' => t('Import data'),
  //   '#name' => 'import',
  // );

  return $form;
}

function cubiconews_emails_form_validate($form, &$form_state) {
}

function cubiconews_emails_form_submit ($form, &$form_state) {
}


function cubiconews_addemails_form($form, &$form_submit) {
	$form = array();
	//$form['block_message'] =array(
	//	'#markup' => t('In case you do not want to receive our newsletters anymore, insert your email at the following field to unsubscribe.'),
	//);
	$form['emails_area'] = array(
		'#type' => 'textarea',
		'#title' => t('Emails'),
		'#description' => t('Copy here all the emails you want to import. It doesn\'t matter how the text looks like. The script will recognise the emails and add the new ones in the database. If there are emails that already exist, then these will be enabled again.'),
		'#default_value' => '',
		'#required' => true
	);

	$form['btn_submit'] = array(
		'#type' => 'submit',
		'#value' => t('Import'),
		'#name' => 'save',
	);

	return $form;
}

function cubiconews_addemails_form_validate($form, &$form_state) {
}

function cubiconews_addemails_form_submit ($form, &$form_state) {
	$emails_address = $form_state['values']['emails_area'];
	$extracted_emails = _extract_emails($emails_address);

	$emails = array_unique($extracted_emails[0]);
	$add_cnt = 0;
	$upd_cnt = 0;
	foreach($emails AS $curemail) {
		$ret = _register_email($curemail);
		if($ret == 'new') { // email added in database
			$add_cnt++;
		}
		if($ret == 'upd') { // email updated and enabled
			$upd_cnt++;
		}
	}
	//watchdog('cubiconews', '%adds emails added! %upds emails updated!', array('%adds' => $add_cnt, '%upds' => $upd_cnt), WATCHDOG_INFO);

	drupal_set_message(t('@adds emails added! @upds emails updated!', array('@adds' =>$add_cnt, '@upds' => $upd_cnt)), 'status');
	//drupal_goto('cubiconews/newsletters/emails');
}
