<?php
// $Id: audio.theme.inc,v 1.3 2008/12/22 08:47:39 drewish Exp $

/**
 * Format the teaser for an audio node.
 */
function theme_audio_teaser($node) {
  $format = variable_get('audio_teaser_format', '[audio-player]<br />[audio-length]');
  return token_replace($format, 'node', $node);
}

/**
 * Format the audio node for display
 */
function theme_audio_display($node) {
  $items = array();
  if ($player = audio_get_node_player($node)) {
    $items[] = $player;
  }
  foreach (audio_get_tag_settings() as $tag => $setting) {
    if (!$setting['hidden'] && isset($node->audio_tags[$tag])) {
      $items[] = '<strong>'. t(ucwords(str_replace('_', ' ', $tag))) .':</strong> '
        . theme('audio_format_tag', $tag, $node->audio_tags[$tag], $setting);
    }
  }
  $items[] = '<strong>'. t('Length') .':</strong> '. theme('audio_format_filelength', $node->audio);
  $items[] = '<strong>'. t('Format') .':</strong> '. theme('audio_format_fileformat', $node->audio);

  $output = "<div class='audio-node block'>\n";
  // give audio_image.module (or a theme) a chance to display the images.
  if (isset($node->audio_images)) {
    $output .= theme('audio_images', $node->audio_images);
  }
  $output .= theme('item_list', $items, NULL, 'ul', array('class' => 'audio-info'));
  $output .= "</div>\n";

  return $output ;
}

/**
 * Format a metadata tag.
 */
function theme_audio_format_tag($tag, $value, $setting) {
  if ($setting['browsable']) {
    return l($value, "audio/by/$tag/". audio_clean_tag($value));
  }
  else {
    switch ($tag) {
      // url tags
      case 'url_file':
      case 'url_artist':
      case 'url_source':
      case 'url_station':
        return '<a href="'. check_url($value) .'" title="'. $tag .'">'. check_plain($value) .'</a>';

      default:
        return check_plain($value);
    }
  }
}

/**
 * Return a string describing the node's file size and play time.
 *
 * @param $fileinfo
 *   Audio node's audio array.
 */
function theme_audio_format_filelength($fileinfo) {
  $args['@filesize'] = format_size(isset($fileinfo['file']->filesize) ? $fileinfo['file']->filesize : 0);
  if (empty($fileinfo['playtime'])) {
    return t('@filesize', $args);
  }
  else {
    $args['@playtime'] = $fileinfo['playtime'];
    return t('@playtime minutes (@filesize)', $args);
  }
}

/**
 * Return a string describing the node's file information.
 *
 * @param $fileinfo
 *   Audio node's audioinfo array.
 */
function theme_audio_format_fileformat($fileinfo) {
  $format = '';
  $args = array();
  if (!empty($fileinfo['format'])) {
    $format .= '@format ';
    $args['@format'] = check_plain(drupal_strtoupper($fileinfo['format']));
  }
  if (!empty($fileinfo['channel_mode'])) {
    $format .= '@channel_mode ';
    $args['@channel_mode'] = check_plain(drupal_ucfirst($fileinfo['channel_mode']));
  }
  if (!empty($fileinfo['sample_rate'])) {
    $format .= '@sampleratekHz ';
    $args['@samplerate'] = (integer) ($fileinfo['sample_rate'] / 1000);
  }
  if (!empty($fileinfo['bitrate'])) {
    $format .= '@bitrateKbps ';
    $args['@bitrate'] = (integer) ($fileinfo['bitrate'] / 1000);

    if (!empty($fileinfo['bitrate_mode'])) {
      $format .= '(@bitrate_mode)';
      $args['@bitrate_mode'] = check_plain(drupal_strtoupper($fileinfo['bitrate_mode']));
    }
  }
  return strtr($format, $args);
}

