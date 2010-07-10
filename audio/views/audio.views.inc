<?php
// $Id: audio.views.inc,v 1.2 2009/11/25 19:27:24 drewish Exp $


/**
 * Implementation of hook_views_data()
 */
function audio_views_data() {
  // Basic table information.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['audio']['table']['group']  = t('Audio');

  // For other base tables, explain how we join
  $data['audio']['table']['join'] = array(
    'node' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
    'files' => array(
      'left_field' => 'fid',
      'field' => 'fid',
    ),
  );

  // ----------------------------------------------------------------
  // Fields

  $data['audio']['play_count'] = array(
    'title' => t('Play count'),
    'help' => t('This will display the number of times this has been played.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
     'filter' => array(
       'handler' => 'views_handler_filter_numeric',
       'label' => t('Plays'),
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  $data['audio']['download_count'] = array(
    'title' => t('Download count'),
    'help' => t('This will display the number of times this has been downloaded.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
     'filter' => array(
       'handler' => 'views_handler_filter_numeric',
       'label' => t('Plays'),
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['audio']['playtime'] = array(
    'title' => t('Length'),
    'help' => t('The playing time of the audio.'), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['audio']['downloadable'] = array(
    'title' => t('Downloadable'),
    'help' => t('Whether or not the audio is downloadable.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Downloadable'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  $data['audio']['player'] = array(
    'title' => t('Player'),
    'help' => t('This will display a flash player.'),
    'real field' => 'nid',
    'field' => array(
      'handler' => 'audio_handler_field_player',
      'additional fields' => array('nid', 'fid'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  $data['audio']['play_link'] = array(
    'title' => t('Play link'),
    'help' => t('This will display a player link link.'),
    'real field' => 'fid',
    'field' => array(
      'handler' => 'audio_handler_field_play_link',
      'additional fields' => array('nid', 'fid'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['audio']['download_link'] = array(
    'title' => t('Download link'),
    'help' => t('This will display a download link if the node allows it.'),
    'real field' => 'fid',
    'field' => array(
      'handler' => 'audio_handler_field_download_link',
      'additional fields' => array('nid', 'fid', 'downloadable'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  foreach (audio_get_tags_allowed() as $tag) {
    $data['audio_metadata_'. $tag]['table']['group']  = t('Audio');

    // For other base tables, explain how we join
    $data['audio_metadata_'. $tag]['table']['join'] = array(
      'node' => array(
        'left_field' => 'vid',
        'table' => 'audio_metadata',
        'field' => 'vid',
        'extra' => array(array('field' => 'tag', 'value' => $tag, 'operator' => '='))
       ),
      'node_revisions' => array(
        'left_field' => 'vid',
        'table' => 'audio_metadata',
        'field' => 'vid',
        'extra' => array(array('field' => 'tag', 'value' => $tag, 'operator' => '='))
       ),
    );

    $data['audio_metadata_'. $tag]['value'] = array(
      'title' => t('@tag', array('@tag' => drupal_ucfirst($tag))),
      'help' => t('The value of the @tag metadata field.', array('@tag' => $tag)),
      'field' => array(
        'handler' => 'views_handler_field_xss',
        'click sortable' => TRUE,
       ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    );
/*
    $data['audio_metadata_'. $tag]['clean'] = array(
      'title' => t('Cleaned @tag value', array('@tag' => $tag)),
      'help' => t('The stripped down version of the @tag metadata field.', array('@tag' => $tag)),
      'field' => array(
        'handler' => 'views_handler_field_xss',
        'click sortable' => TRUE,
       ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    );
*/
  }

  $data['audio']['format'] = array(
    'title' => t('Format'),
    'help' => t('The file format of the audio.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  
  return $data;
}


/**
 * Implementation of hook_views_handlers()
 */
function audio_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'audio') . '/views/handlers',
    ),
    'handlers' => array(
      // field handlers
      'audio_handler_field_download_link' => array(
        'parent' => 'views_handler_field',
      ),
      'audio_handler_field_play_link' => array(
        'parent' => 'views_handler_field',
      ),
      'audio_handler_field_player' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}

