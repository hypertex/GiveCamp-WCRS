<?php
// $Id: xspf_slim.inc,v 1.5 2007/07/31 03:17:23 drewish Exp $

function audio_xspf_slim_audio_player() {
  $items['xspf_slim'] = array(
    'module'      => 'audio',
    'title'       => t('XSPF Web Music Player - Slim'),
    'description' => t('Slim version 0.2.3'),
    'url'         => t('http://musicplayer.sourceforge.net/'),
    'preview'     => 'players/xspf_slim.png',
    'formats'     => array('wav', 'mp3'),
    'theme_node'  => 'audio_xspf_slim_node_player',
    'theme_xspf'  => 'audio_xspf_slim_xspf_player',
  );
  return $items;
}

function theme_audio_xspf_slim_node_player($node, $options = array()) {
  // make sure it's compatible with the flash player
  if (!audio_is_flash_playable($node)) {
    return NULL;
  }

  $options['song_url'] = check_url($node->url_play);
  $options['song_title'] = check_plain($node->audio_tags['title']);

  return audio_xspf_slim_player($options);
}

function theme_audio_xspf_slim_xspf_player($path, $options = array()) {
  $options['playlist_url'] = check_url($path);

  return audio_xspf_slim_player($options);
}

function audio_xspf_slim_player($options) {

  $url = base_path() . drupal_get_path('module', 'audio') .'/players/xspf_slim.swf';
  $flashvars = audio_query_string_encode($options);

$output = <<<EOT
<object type="application/x-shockwave-flash" data="$url" width="400" height="15">
  <param name="movie" value="$url" />
  <param name="menu" value="false" />
  <param name="quality" value="high" />
  <param name="FlashVars" value="$flashvars" />
  <embed src="$url" flashvars="$flashvars" width="400" height="15" />
</object>
EOT;

  return $output;
}
