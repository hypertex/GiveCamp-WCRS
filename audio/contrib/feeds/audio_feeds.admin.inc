<?php
// $Id: audio_feeds.admin.inc,v 1.1 2009/11/28 01:38:42 drewish Exp $

/**
 * Settings form.
 */
function audio_feeds_admin() {
  $form['audio_feeds_feed_links'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Display feed links'),
    '#options' => array('m3u' => t('M3U'), 'pls' => t('PLS'), 'xspf' => t('XSPF')),
    '#default_value' => variable_get('audio_feeds_feed_links', array('m3u', 'pls', 'xspf')),
    '#description' => t('If audio feeds are enabled on some content types, only those links that are checked here will be displayed. Keep in mind that this is only a display option and that clever users might still be able to find the paths of feeds that are not displayed.'),
  );

  return system_settings_form($form);
}
