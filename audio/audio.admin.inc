<?php
// $Id: audio.admin.inc,v 1.4 2009/11/25 19:29:47 drewish Exp $

/**
 * Settings form.
 */
function audio_admin_settings() {
  $form['audio_default_title_format'] = array(
    '#type' => 'textfield',
    '#title' => t('Default node title format'),
    '#maxlength' => 128,
    '#default_value' => variable_get('audio_default_title_format', '[audio-tag-title-raw] by [audio-tag-artist-raw]'),
    '#description' => t("The audio node's title can use the file's metadata as variables. This will be used as the default title for all new audio nodes. By using the tokens listed below, you can automatically create titles from things like a song's artist or title. <strong>Note:</strong> the node title is escaped so it is safe to use the -raw tokens."),
  );
  $form['audio_teaser_format'] = array(
    '#type' => 'textfield',
    '#title' => t('Node teaser format'),
    '#maxlength' => 128,
    '#default_value' => variable_get('audio_teaser_format', '[audio-player]<br />[audio-length]'),
    '#description' => t("Use this setting to customize the teasers for audio nodes. Using the tokens listed below you can select what information about the file will be displayed. <strong>Note: the teaser is not escaped so it is unsafe to use the -raw tokens.</strong>"),
  );
  $form['token_help'] = array(
    '#title' => t('List of available tokens'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('This is a list of the tokens that can be used in the titles and teasers of audio nodes.'),
    'help' => array('#value' => theme('token_help', 'node')),
  );
  $form['audio_allowed_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Permitted audio file extensions'),
    '#maxlength' => 128,
    '#default_value' => variable_get('audio_allowed_extensions', 'mp3 wav ogg'),
    '#description' => t('Audio file extensions that users can upload. Separate extensions with a space and do not include a leading dot.'),
  );
  $form['audio_default_downloadable'] = array(
    '#type' => 'checkbox',
    '#title' => t("Downloadable by default"),
    '#default_value' => variable_get('audio_default_downloadable', 1),
    '#description' => t('Check this to make downloadable the default setting for new audio nodes. You should be aware that even when audio is not marked as downloadable, clever users can still download it, this just makes the work harder. '),
  );

  return system_settings_form($form);
}

function audio_admin_settings_validate($form, &$form_state) {
  // Ensure they don't try to slip raw tokens in.
  if (preg_match("/\[.*?\-raw\s*?\]/i", $form_state['values']['audio_teaser_format'])) {
    form_set_error('audio_teaser_format', t('Raw tokens are not allowed.'));
  }
}

/**
 * The ID3 tag settings page.
 */
function audio_admin_settings_metadata() {
  $settings = audio_get_tag_settings();
  $form['audio_tag_settings'] = array(
    '#tree' => TRUE,
    '#theme' => 'audio_admin_settings_metadata_table',
  );
  foreach ($settings as $tag => $setting) {
    $form['audio_tag_settings'][$tag]['name'] = array(
      '#type' => 'item',
      '#value' => filter_xss($tag)
    );
    $form['audio_tag_settings'][$tag]['autocomplete'] = array(
      '#type' => 'checkbox',
      '#default_value' => $setting['autocomplete']
    );
    $form['audio_tag_settings'][$tag]['required'] = array(
      '#type' => 'checkbox',
      '#default_value' => $setting['required'],
    );
    $form['audio_tag_settings'][$tag]['hidden'] = array(
      '#type' => 'checkbox',
      '#default_value' => $setting['hidden'],
    );
    $form['audio_tag_settings'][$tag]['browsable'] = array(
      '#type' => 'checkbox',
      '#default_value' => $setting['browsable'],
    );
    $form['audio_tag_settings'][$tag]['writetofile'] = array(
      '#type' => 'checkbox',
      '#default_value' => $setting['writetofile'],
    );
    $form['audio_tag_settings'][$tag]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $setting['weight']
    );
    $form['audio_tag_settings'][$tag]['delete'] = array(
      '#type' => 'checkbox',
      '#default_value' => FALSE,
    );
  }

  // Add in a row for a new tag.
  $form['audio_tag_settings']['new']['name'] = array(
    '#type' => 'textfield',
    '#size' => 15,
    '#maxlength' => 45,
  );
  $form['audio_tag_settings']['new']['autocomplete'] = array(
    '#type' => 'checkbox',
    '#default_value' => TRUE,
  );
  $form['audio_tag_settings']['new']['required'] = array(
    '#type' => 'checkbox',
    '#default_value' => FALSE,
  );
  $form['audio_tag_settings']['new']['hidden'] = array(
    '#type' => 'checkbox',
    '#default_value' => FALSE,
  );
  $form['audio_tag_settings']['new']['browsable'] = array(
    '#type' => 'checkbox',
    '#default_value' => TRUE,
  );
  $form['audio_tag_settings']['new']['writetofile'] = array(
    '#type' => 'checkbox',
    '#default_value' => FALSE,
  );
  $form['audio_tag_settings']['new']['weight'] = array(
    '#type' => 'weight',
    '#default_value' => 10,
  );
  $form['audio_tag_settings']['new']['delete'] = array(
    '#type' => 'value',
    '#value' => FALSE,
  );

  $form['buttons']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration') );
  $form['buttons']['reset'] = array('#type' => 'submit', '#value' => t('Reset to defaults') );

  return $form;
}

/**
 * Save the ID3 tag settings, we can't use system_settings_form_submit() as the
 * form callback because it wouldn't call theme_audio_settings().
 */
function audio_admin_settings_metadata_submit($form, $form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';

  if ($op == t('Reset to defaults')) {
    variable_del('audio_tag_settings');
    drupal_set_message(t('The configuration options have been reset to their default values.'));
  }
  else {
    // Remove any deleted tags.
    foreach ($form_state['values']['audio_tag_settings'] as $name => $field) {
      if ($form_state['values']['audio_tag_settings'][$name]['delete']) {
        unset($form_state['values']['audio_tag_settings'][$name]);
      }
    }
    // If there's a new tag, add it in.
    if ($form_state['values']['audio_tag_settings']['new']['name']) {
      $name = $form_state['values']['audio_tag_settings']['new']['name'];
      $form_state['values']['audio_tag_settings'][$name] = $form_state['values']['audio_tag_settings']['new'];
    }
    unset($form_state['values']['audio_tag_settings']['new']);

    // Sort by the weight and then by name of the tag. I'm sure there's a
    // better way to do this...
    foreach ($form_state['values']['audio_tag_settings'] as $tag => $settings) {
      $weights[$tag] = $settings['weight'];
      $names[$tag] = $tag;
    }
    array_multisort($weights, SORT_ASC, SORT_NUMERIC, $names, SORT_ASC, SORT_STRING);
    $audio_tag_settings = array();
    foreach ($names as $tag) {
      $audio_tag_settings[$tag] = $form_state['values']['audio_tag_settings'][$tag];
    }

    // ...and save it.
    variable_set('audio_tag_settings', $audio_tag_settings);

    drupal_set_message(t('The configuration options have been saved.'));
  }
}

/**
 * Format the id3tags settings form as a table.
 */
function theme_audio_admin_settings_metadata_table($form_element) {
  $rows = array();
  foreach (element_children($form_element) as $key) {
    $form_element[$key]['weight']['#attributes']['class'] = 'metadata-tag-weight';

    $rows[] = array(
      'data' => array(
        drupal_render($form_element[$key]['name']),
        drupal_render($form_element[$key]['autocomplete']),
        drupal_render($form_element[$key]['required']),
        drupal_render($form_element[$key]['hidden']),
        drupal_render($form_element[$key]['browsable']),
        drupal_render($form_element[$key]['writetofile']),
        drupal_render($form_element[$key]['weight']),
        drupal_render($form_element[$key]['delete']),
      ),
      'class' => 'draggable',
    );
  }
  $header = array(t('Tag'), t('Autocompleted'), t('Required'), t('Hidden'), t('Browsable'),
    t('Written to file'), t('Weight'), t('Delete'));

  $output = '';
  if (count($rows)) {
    drupal_add_tabledrag('audio-settings-metadata', 'order', 'sibling', 'metadata-tag-weight');
    $output .= theme('table', $header, $rows, array('id' => 'audio-settings-metadata'));
  }
  $output .= drupal_render($form_element);
  return $output;
}

/**
 * Form for player settings.
 */
function audio_admin_settings_players() {
  $form['player_settings']['#theme'] = 'audio_admin_settings_players';

  $options = array();
  foreach (audio_get_players('formats') as $format => $players) {
    foreach ($players as $id => $player) {
      $options[$id] = ''; // Put no label on options.
      $form['player_settings']['players'][$format][$id]['title'] = array(
        '#type' => 'item',
        '#title' => t('Title'),
        '#value' => $player['title'],
      );
      $form['player_settings']['players'][$format][$id]['description'] = array(
        '#type' => 'item',
        '#title' => t('Description'),
        '#value' => $player['description'],
      );
      $form['player_settings']['players'][$format][$id]['url'] = array(
        '#type' => 'item',
        '#title' => t('URL'),
        '#value' => $player['url'],
      );
      $form['player_settings']['players'][$format][$id]['preview'] = array(
        '#type' => 'item',
        '#title' => t('URL'),
        '#value' => drupal_get_path('module', $player['module']) .'/'. $player['preview'],
      );
    }
    $form['player_settings']['audio_player_'. $format] = array(
      '#type' => 'radios',
      '#title' => t('Player'),
      '#default_value' => variable_get('audio_player_'. $format, '1pixelout'),
      '#options' => $options,
    );
  }
  return system_settings_form($form);
}

function theme_audio_admin_settings_players($form_element) {
  $output = '';
  $header = array(t('Selection'), t('Player'), t('Description'), t('Homepage'));
  foreach (element_children($form_element['players']) as $format) {
    $output .= '<h2>'. t('%format files', array('%format' => $format)) .'</h2>';
    $rows = array();
    foreach (element_children($form_element['players'][$format]) as $name) {
      $rows[] = array(
        array('data' => drupal_render($form_element['audio_player_'. $format][$name]), 'align' => 'center'),
        check_plain($form_element['players'][$format][$name]['title']['#value']) .'<br />'.
          theme('image', $form_element['players'][$format][$name]['preview']['#value'], 'preview', 'preview'),
        check_plain($form_element['players'][$format][$name]['description']['#value']),
        l(t('Link'), $form_element['players'][$format][$name]['url']['#value']),
      );
      unset($form_element['players'][$format][$name]['title']);
      unset($form_element['players'][$format][$name]['description']);
      unset($form_element['players'][$format][$name]['url']);
      unset($form_element['players'][$format][$name]['preview']);
    }
    $output .= theme('table', $header, $rows);
  }
  return $output . drupal_render($form);
}
