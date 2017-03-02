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

  $registered_emails = 0;
  $cleaned_emails = 0;
	while($record = $result->fetchAssoc()) {
		switch($record['status']) {
			case 0:
        $cleaned_emails = $record['cnt'];
				break;
			case 1:
        $registered_emails = $record['cnt'];
				break;
		}
	}
  $output .=  '<div id="email-stats-wrapper">';
  $output .=  '<div class="enabled email-stats-box">' . $registered_emails . '<br><span class="label">' . t('Registered Emails') . '</span></div>';
  $output .=  '<div class="disabled email-stats-box">' . $cleaned_emails . '<br><span class="label">' . t('Cleaned Emails') . '</span></div>';
  $output .=  '</div>';


  $form['email_list_info'] = array(
    '#markup' => $output,
  );


  // Build the 'Update options' form.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
    '#access' => user_access('administer cubiconews emails'),
  );
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => array(
      'enable' => 'Enable selected emails',
      'disable'=> 'Disable selected emails',
      'delete' => 'Remove selected emails',
    ),
    '#default_value' => 'enable',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );


  $header = array(
    'email' => array('data' => t('Email Address'), 'field' => 'cube.email', 'sort' => 'asc'),
    'name' => array('data' => t('First Name'), 'field' => 'cube.name'),
    'surname' => array('data' => t('Last Name'), 'field' => 'cube.surname'),
    'status' => array('data' => t('Status'), 'field' => 'cube.status'),
    'registration' => array('data' => t('Date Added'), 'field' => 'cube.registration'),
    //'operations' => array('data' => t('Operations')),
  );

  $query = db_select('cubiconews_emails', 'cube')->extend('PagerDefault')->extend('TableSort');
  $query->fields('cube',array('eid', 'name', 'surname', 'email', 'registration', 'status'))
    ->limit(50)
    ->orderByHeader($header);
  $result = $query->execute();

  $options = array();
  while($record = $result->fetchAssoc()) {
    $options[$record['eid']] = array(
      'email' => $record['email'],
      'name' => $record['name'],
      'surname' => $record['surname'],
      'status' => $record['status'] ? t('Subscribed') : t('Cleaned'),
      'registration' => format_date($record['registration'], 'short'),
    );
  }

  $form['emails'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No content available.'),
  );

  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}

function cubiconews_emails_form_validate($form, &$form_state) {
}

function cubiconews_emails_form_submit ($form, &$form_state) {
  switch($form_state['values']['operation']) {
    case 'enable':
      $disable_email = db_update('cubiconews_emails')
        ->fields(array(
            'status' => 1
          ))
        ->condition('eid', array_values($form_state['values']['emails']), 'IN')
        ->execute();
      break;
    case 'disable':
      $disable_email = db_update('cubiconews_emails')
        ->fields(array(
            'status' => 0
          ))
        ->condition('eid', array_values($form_state['values']['emails']), 'IN')
        ->execute();
      break;
    case 'delete':
      db_delete('cubiconews_emails')
        ->condition('eid', array_values($form_state['values']['emails']), 'IN')
        ->execute();
      break;
  }
}


function cubiconews_addemails_form($form, &$form_submit) {
	$form = array();
	$form['emails_area'] = array(
		'#type' => 'textarea',
		'#title' => t('Emails'),
		'#description' => t('Copy here all the emails you want to import. It doesn\'t matter how the text looks like. The script will recognise the emails and add the new ones in the database. If there are emails that already exist, then these will be enabled again.'),
		'#default_value' => '',
		'#required' => true
	);
	$form['enable_existing'] = array(
		'#type' => 'checkbox',
		'#title' => t('Enable existing emails'),
		'#description' => t('If selected, the existing cleaned emails will be enabled again.'),
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
  $enable_existing = $form_state['values']['enable_existing'];

	$extracted_emails = _extract_emails($emails_address, $enable_existing);

	$emails = array_unique($extracted_emails[0]);
	$add_cnt = 0;
	$upd_cnt = 0;
	foreach($emails AS $curemail) {
		$ret = _register_email($curemail, $enable_existing);
		if($ret == 'new') { // email added in database
			$add_cnt++;
		}
		if($ret == 'upd') { // email updated and enabled
			$upd_cnt++;
		}
	}
	//watchdog('cubiconews', '%adds emails added! %upds emails updated!', array('%adds' => $add_cnt, '%upds' => $upd_cnt), WATCHDOG_INFO);

	drupal_set_message(t('@adds emails added! @upds emails updated!', array('@adds' =>$add_cnt, '@upds' => $upd_cnt)), 'status');
}
