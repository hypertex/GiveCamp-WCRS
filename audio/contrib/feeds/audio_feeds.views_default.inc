<?php
// $Id: audio_feeds.views_default.inc,v 1.2 2008/10/03 06:39:16 drewish Exp $

/**
 *  Implementation of hook_views_default_tables()
 */
function audio_feeds_views_default_views() {
/*
# I UPDATED THIS TO A VIEWS2 VIEW BUT HAVEN'T CONVERTED THE STYLE PLUGIN HANDLER
# IT DOESN'T MAKE ANY SENSE TO UNCOMMENT THIS UNTIL THAT'S CONVERTED.
  $view = new view;
  $view->name = 'audio_feeds';
  $view->description = 'Audio XSPF Player';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'title' => array(
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
    ),
  ));
  $handler->override_option('sorts', array(
    'created' => array(
      'id' => 'created',
      'table' => 'node',
      'field' => 'created',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'audio' => 'audio',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'role',
    'role' => array(
      '1' => '1',
      '2' => '2',
    ),
    'perm' => '',
  ));
  $handler->override_option('title', 'Listening Station');
  $handler->override_option('header_format', '1');
  $handler->override_option('footer_format', '1');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', '15');
  $handler->override_option('style_plugin', 'list');
  $handler->override_option('style_options', array(
    'type' => 'ol',
  ));
  $handler->override_option('row_plugin', 'node');
  $handler->override_option('row_options', array(
    'teaser' => FALSE,
    'links' => TRUE,
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'audio/listen');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler = $view->new_display('feed', 'Feed', 'feed_1');
  $handler->override_option('style_plugin', 'rss');
  $handler->override_option('style_options', array(
    'mission_description' => FALSE,
    'description' => '',
  ));
  $handler->override_option('row_plugin', 'node_rss');
  $handler->override_option('row_options', array());
  $handler->override_option('path', 'audio/listen/feed');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler->override_option('displays', array(
    'default' => 'default',
    'page_1' => 'page_1',
  ));
  $handler->override_option('sitename_title', FALSE);
 */
  $view = new stdClass();
  $view->name = 'audio_feeds';
  $view->description = 'Audio XSPF Player';
  $view->access = array(
    0 => '1',
    1 => '2',
  );
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = 'Listening Station';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'audio_xspf_playlist';
  $view->url = 'audio/listen';
  $view->use_pager = FALSE;
  $view->nodes_per_page = '15';
  $view->sort = array(
    array(
      'tablename' => 'node',
      'field' => 'created',
      'sortorder' => 'DESC',
      'options' => 'normal',
    ),
  );
  $view->argument = array(
    array(
      'type' => 'rss_feed',
      'argdefault' => '2',
      'title' => '',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array(
    array(
      'tablename' => 'node',
      'field' => 'title',
      'label' => '',
      'handler' => 'views_handler_field_nodelink_with_mark',
      'options' => 'link',
    ),
  );
  $view->filter = array(
    array(
      'tablename' => 'node',
      'field' => 'type',
      'operator' => 'OR',
      'options' => '',
      'value' => array(
          0 => 'audio',
        ),
    ),
    array(
      'tablename' => 'node',
      'field' => 'status',
      'operator' => '=',
      'options' => '',
      'value' => '1',
    ),
  );
  $view->exposed_filter = array();
  $view->requires = array(node);
  $views[$view->name] = $view;

  return $views;
}
