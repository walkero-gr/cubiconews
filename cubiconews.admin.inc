<?php
/**
 * Config Admin page
 *
 */
function _cubiconews_admin_preferences_form($form, &$form_state){
	$form = array();
  $form['page_info'] = array(
    '#markup' => '<h1>' . t('CubicoNews configuration') . '</h1>'
  );

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

  $systemEmail = variable_get('system_mail');
	$form['cubiconews_config_sender_email'] = array(
		'#type' => 'textfield',
		'#title' => 'Sender email',
		'#default_value' => variable_get('cubiconews_config_sender_email', $systemEmail),
		'#size' => 60,
		'#required' => TRUE,
		'#description' => 'Set the sender email account from which the newsletter is sent.'
	);

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
function _cubiconews_admin_preferences_form_validate($form, &$form_state) {
  $email_address = _extract_emails($form_state['values']['cubiconews_config_sender_email']);

	if (empty($email_address[0][0])) {
    form_set_error('cubiconews_config_sender_email', t('Your email address is not valid.'));
	} else {
    $form_state['values']['cubiconews_config_sender_email'] = $email_address[0][0];
  }
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
	variable_set('cubiconews_config_sender_email', $form_state['values']['cubiconews_config_sender_email']);
	variable_set('cubiconews_config_test_emails', $form_state['values']['cubiconews_config_test_emails']);

	drupal_set_message(t('Configuration saved succesfully.'), 'status');
}


/**
 * Manage emails admin page
 *
 */
function cubiconews_emails_form($form, &$form_state) {
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


  $form['page_info'] = array(
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


/**
 * Import emails admin page
 *
 */
function cubiconews_addemails_form($form, &$form_state) {
	$form = array();
  $form['page_info'] = array(
    '#markup' => '<h1>Import multiple emails</h1>',
  );
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
		'#value' => t('Import emails'),
		'#name' => 'save',
	);

	return $form;
}

function cubiconews_addemails_form_validate($form, &$form_state) {
}

function cubiconews_addemails_form_submit ($form, &$form_state) {
	$emails_address = $form_state['values']['emails_area'];
  $enable_existing = $form_state['values']['enable_existing'];

	$extracted_emails = _extract_emails($emails_address);

	$emails = array_unique($extracted_emails[0]);
  $emails_num = count($emails);

  // Set up batch
  $batch = array(
    'operations' => array(),
    'title' => t('Cubiconews: Import emails'),
    'init_message' => t('Import emails is starting.'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Import emails has encountered an error.'),
    'finished' => '_cubiconews_import_emails_batch_finished'
  );

  $progress = 0; // where to start
  $limit = 500; // how many to process for each run
  $max = $emails_num; // how many records to process until stop - can do query here to get max times to run
  while ($progress <= $max) {
    $batch['operations'][] = array('_cubiconews_import_emails_batch_process', array($progress, $limit, $emails, $enable_existing));
    $progress = $progress + $limit;
  }
  batch_set($batch);
}


/**
 * Manage newsletters admin page
 *
 */
function cubiconews_newsletters_form($form, &$form_state) {
  $form = array();
  $form['page_info'] = array(
    '#markup' => '<h1>' . t('Manage Newsletters') . '</h1>'
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
      'delete' => 'Remove selected newsletters',
    ),
    '#default_value' => 'delete',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'n.title'),
    'status' => array('data' => t('Status'), 'field' => 'cubnl.status'),
    'created' => array('data' => t('Date Added'), 'field' => 'n.created', 'sort' => 'desc'),
    'sent' => array('data' => t('Date Sent'), 'field' => 'cubnl.sent'),
    'schedule' => array('data' => t('Scheduled'), 'field' => 'cubnl.schedule'),
    'operations' => array('data' => t('Operations')),
    'statistics' => array('data' => t('Statistics')),
  );

  $query = db_select('cubiconews_newsletters', 'cubnl')->extend('PagerDefault')->extend('TableSort');
  $query->fields('n', array('nid', 'title', 'created', 'changed'))
    ->fields('cubnl', array('status', 'sent', 'schedule'));
  $query->join('node', 'n', 'n.nid = cubnl.nid');
  $query->condition('n.type', 'cubiconews_item', '=')
    ->condition('n.status', 1, '=')
    ->limit(50)
    ->orderByHeader($header);

	$result = $query->execute();

  $languages = language_list();
  $options = array();
  while($record = $result->fetchAssoc()) {
    switch($record['status']) {
      case -1:
        $status_text = 'Cancelled';
        break;
      case 0:
        $status_text = 'Unsent / New';
        break;
      case 1:
        $status_text = 'Scheduled';
        break;
      case 9:
        $status_text = 'Sent';
        break;
    }
    $node = node_load($record['nid']);
    $langcode = entity_language('node', $node);
    $uri = entity_uri('node', $node);
    if ($langcode != LANGUAGE_NONE && isset($languages[$langcode])) {
      $uri['options']['language'] = $languages[$langcode];
    }
    $options[$record['nid']] = array(
      'title' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $record['title'],
          '#href' => $uri['path'],
          '#options' => $uri['options'],
          '#suffix' => ' ' . theme('mark', array('type' => node_mark($record['nid'], $record['changed']))),
        ),
      ),
      'status' => $status_text,
      'created' => format_date($record['created'], 'short'),
      'sent' => ((!empty($record['sent'])) ? format_date($record['sent'], 'short') : '-'),
      'schedule' => ((!empty($record['schedule'])) ? format_date($record['schedule'], 'short') : '-'),
      'operations' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('Send newsletter'),
          '#href' => 'admin/config/content/cubiconews/' . $record['nid'] . '/send',
          //'#options' => $uri['options'],
          //'#suffix' => ' ' . theme('mark', array('type' => node_mark($record['nid'], $record['changed']))),
        ),
			),
			'statistics' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('Statistics'),
          '#href' => 'admin/config/content/cubiconews/' . $record['nid'] . '/stats'
        ),
      ),
    );
  }

  $form['newsletters'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No content available.'),
  );

  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}

function cubiconews_newsletters_form_validate($form, &$form_state) {
}

function cubiconews_newsletters_form_submit ($form, &$form_state) {
  switch($form_state['values']['operation']) {
    case 'delete':
      node_delete_multiple($form_state['values']['newsletters']);
      break;
  }
}


/**
 * Delete multiple emails admin page
 *
 */
function cubiconews_removeemails_form($form, &$form_state) {
	$form = array();
  $form['page_info'] = array(
    '#markup' => '<h1>Remove multiple emails</h1>',
  );
	$form['emails_area'] = array(
		'#type' => 'textarea',
		'#title' => t('Emails'),
		'#description' => t('Copy here all the emails you want to delete from the database. It doesn\'t matter how the text looks like. The script will recognise the emails and remove them.'),
		'#default_value' => '',
		'#required' => true
	);
	$form['approval'] = array(
		'#type' => 'checkbox',
		'#title' => t('Are you sure you want to delete these email accounts? This cannot be undone.'),
		'#required' => true
	);

	$form['btn_submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete emails'),
		'#name' => 'save',
	);

	return $form;
}

function cubiconews_removeemails_form_validate($form, &$form_state) {
}

function cubiconews_removeemails_form_submit ($form, &$form_state) {
	$emails_address = $form_state['values']['emails_area'];
  $enable_existing = $form_state['values']['enable_existing'];

	$extracted_emails = _extract_emails($emails_address);

	$emails = array_unique($extracted_emails[0]);

  db_delete('cubiconews_emails')
    ->condition('email', $emails, 'IN')
    ->execute();

	drupal_set_message(t('@cnt emails deleted from the database!', array('@cnt' => count($emails))), 'status');
}


/**
 * Sent newsletter admin page
 *
 */
function cubiconews_newsletter_send_form($form, &$form_state) {
	$form = array();
  $form['page_info'] = array(
    '#markup' => '<h1>Send newsletter</h1>',
  );


	$form['schedule_checkbox'] = array(
		'#type' => 'checkbox',
		'#title' => t('Schedule to send the newsletter later.'),
	);
  $form['schedule_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Schedule Date & Time'),
    '#date_format' => 'Y-m-d H:i',
    '#date_year_range' => '0:1',
		'#datepicker_options' => array(
      'minDate' => date('Y-m-d H:i'),
      'maxDate' => date('Y-m-d H:i', strtotime('today +1 month midnight'))
    ),
    '#states' => array(
      'visible' => array(
        ':input[name="schedule_checkbox"]' => array("checked" => TRUE)
      ),
      'required' => array(
        ':input[name="schedule_checkbox"]' => array("checked" => TRUE)
      )
    )
  );

  $form['btn_sendtest'] = array(
    '#type' => 'submit',
    '#value' => t('Send test email'),
    '#name' => 'sendtest',
  );
	$form['btn_schedule'] = array(
		'#type' => 'submit',
		'#value' => t('Schedule'),
		'#name' => 'schedule',
    '#states' => array('visible' => array(':input[name="schedule_checkbox"]' => array("checked" => TRUE))),
	);
	$form['btn_sendnow'] = array(
		'#type' => 'submit',
		'#value' => t('Send now'),
		'#name' => 'sendnow',
    '#states' => array('visible' => array(':input[name="schedule_checkbox"]' => array("checked" => false))),
	);

	return $form;
}

function cubiconews_newsletter_send_form_validate($form, &$form_state) {

  switch($form_state['clicked_button']['#name']) {
    case 'schedule':
      if(empty($form_state['values']['schedule_date'])) {
		    form_set_error('schedule_date', t('If you want to schedule the sending of the newsletter you need to set date and time'));
      }
      break;
    case 'sendnow':
      break;
    case 'sendtest':
      break;
  }
}

function cubiconews_newsletter_send_form_submit ($form, &$form_state) {
  $current_nid = arg(4);

  switch($form_state['clicked_button']['#name']) {
    case 'schedule':
      $schedule_timestamp = strtotime($form_state['values']['schedule_date']);
      $num_updated = db_update('cubiconews_newsletters')
        ->fields(array(
            'schedule' => $schedule_timestamp,
            'status' => 1
          ))
        ->condition('nid', $current_nid, '=')
        ->execute();
        drupal_goto('admin/config/content/cubiconews/newsletters');
      break;
    case 'sendnow':
      $queue = DrupalQueue::get('cubiconews_send_emails_queue');
      $queue->createQueue();

      $query = db_select('cubiconews_emails', 'cube');
      $query->fields('cube',array('eid'))
        ->condition('cube.status', 1, '=');
      $result = $query->execute();
      $emails_num = $result->rowCount();

      // Set up batch
      $batch = array(
        'operations' => array(),
        'title' => t('Cubiconews: Set queues for newsletter sending'),
        'init_message' => t('Setting queues is starting.'),
        'progress_message' => t('Processed @current out of @total.'),
        'error_message' => t('Setting queues has encountered an error.'),
        'finished' => '_cubiconews_build_queues_batch_finished'
      );

      $progress = 0; // where to start
      $limit = 250; // how many to process for each run
      $max = $emails_num; // how many records to process until stop - can do query here to get max times to run
      while ($progress <= $max) {
        $batch['operations'][] = array('_cubiconews_build_queues_batch_process', array($progress, $limit, $current_nid, $queue));
        $progress = $progress + $limit;
      }
      batch_set($batch);
      break;
    case 'sendtest':
      $emails = variable_get('cubiconews_config_test_emails', 'info@intercube.gr');
      $emails_array = _extract_emails($emails);

      foreach($emails_array[0] AS $recipient_email) {
        _cubiconews_send_email($current_nid, $recipient_email);
      }
      drupal_set_message(t('Test email sent to @cnt emails!', array('@cnt' => count($emails_array[0]))), 'status');
      break;
  }
}
