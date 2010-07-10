<?php
// $Id: audio_feeds.feeds.inc,v 1.4 2009/11/25 19:33:13 drewish Exp $

/**
 * @file
 * This file provides various XML feeds for audio media.
 */

/**
 * Menu Callback to generate M3U feed
 */
function audio_feeds_m3u($node) {
  $items = array();

  // prepare feed metadata
  $metadata = array('title' => $node->title);

  // prepare feed items
  $children = audio_feeds_children($node);
  foreach ($children as $audio) {
    $items[] = array(
      'title' => $audio->audio_tags['title'] ? $audio->audio_tags['title'] : $audio->title,
      'author' => $audio->audio_tags['artist'],
      'duration' => $audio->audio_file['playtime'],
      'enclosure' => array('url' => $audio->url_play),
    );
  }
  audio_feeds_generate_m3u($items, $metadata);
}

/**
 * Return XML for M3U feed
 */
function audio_feeds_generate_m3u($items = array(), $metadata = array()) {
  $output = "#EXTM3U\r\n";
  foreach ($items as $item) {
    $title = $item['author'] ? $item['author'] ." - ". $item['title'] : $item['title'];
    $output .= "#EXTINF:". check_plain($item['duration']) .",". check_plain($title) ."\r\n";
    $output .= check_plain($item['enclosure']['url']) ."\r\n";
  }
  drupal_set_header("Pragma: no-cache"); // HTTP/1.0
  drupal_set_header("Cache-Control: private"); // HTTP/1.1
  drupal_set_header("Content-Type: audio/mpegurl", TRUE);
  drupal_set_header("Content-Length: ". strlen($output), TRUE);
  drupal_set_header("Content-Disposition: attachment; filename=\"". check_plain($metadata['title']) .".m3u\"", TRUE);
  print $output;
  exit();
}

/**
 * Menu Callback to generate PLS feed
 */
function audio_feeds_pls($node) {
  $items = array();

  // prepare feed metadata
  $metadata = array('title' => $node->title);

  // prepare feed items
  $children = audio_feeds_children($node);
  foreach ($children as $audio) {
    $items[] = array(
      'title' => $audio->audio_tags['title'] ? $audio->audio_tags['title'] : $audio->title,
      'author' => $audio->audio_tags['artist'],
      'duration' => $audio->audio_file['playtime'],
      'enclosure' => array('url' => $audio->url_play),
    );
  }
  audio_feeds_generate_pls($items, $metadata);
}

/**
 * Return XML for PLS feed
 */
function audio_feeds_generate_pls($items = array(), $metadata = array()) {
  $output = "[playlist]\r\n";
  $i = 1;
  foreach ($items as $item) {
    $title = $item['author'] ? $item['author'] ." - ". $item['title'] : $item['title'];
    $output .= "File$i=". check_plain($item['enclosure']['url']) ."\r\n";
    $output .= "Title$i=". check_plain($title) ."\r\n";
    $output .= "Length$i=". check_plain($item['duration']) ."\r\n";
    $i++;
  }
  $output .= "NumberOfEntries=". count($items) ."\r\n";
  $output .= "Version=2\r\n";
  drupal_set_header("Pragma: no-cache"); // HTTP/1.0
  drupal_set_header("Cache-Control: private"); // HTTP/1.1
  drupal_set_header("Content-Type: audio/x-scpls", TRUE);
  drupal_set_header("Content-Length: ". strlen($output), TRUE);
  drupal_set_header("Content-Disposition: attachment; filename=\"". check_plain($metadata['title']) .".pls\"", TRUE);
  print $output;
  exit();
}

/**
 * Menu Callback to generate XSPF feed
 */
function audio_feeds_xspf($node) {
  global $base_url;
  $children = audio_feeds_children($node);

  // prepare feed metadata
  $metadata = array(
    'title' => $node->title,
    'author' => $node->name,
    'link' => url('node/'. $node->nid, array('absolute' => TRUE)),
    'feed_url' => url('node/'. $node->nid .'/xspf', array('absolute' => TRUE)),
    //'copyright' => $node->playlist_info['copyright']
  );

  // prepare feed items
  $items = array();
  foreach ($children as $audio) {
    // use the first image uploaded as the included image
    $image = is_array($audio->audio_images) ? current($audio->audio_images) : '';
    $items[] = array(
      'title' => $audio->audio_tags['title'] ? $audio->audio_tags['title'] : $audio->title,
      'author' => $audio->audio_tags['artist'],
      'album' => $audio->audio_tags['album'],
      'duration' => $audio->audio_file['playtime'],
      'link' => url('node/'. $audio->nid, array('absolute' => TRUE)),
      'image' => array('url' => $base_url .'/'. $image['filepath']),
      'enclosure' => array('url' => $audio->url_play),
    );
  }
  audio_feeds_generate_xspf($items, $metadata);
}

/**
 * Return XML for XSPF feed
 */
function audio_feeds_generate_xspf($items = array(), $metadata = array()) {
  $output = '<?xml version="1.0" encoding="UTF-8"?>';
  $output .= '<playlist version="1" xmlns="http://xspf.org/ns/0/">';
  $output .= "<title>". check_plain($metadata['title']) ."</title>";
  $output .= "<annotation>". check_plain($metadata['author']) ."</annotation>";
  $output .= "<creator>". check_plain($metadata['author']) ."</creator>";
  $output .= "<info>". check_plain($metadata['link']) ."</info>";
  $output .= "<location>". check_plain($metadata['feed_url']) ."</location>";
  $output .= "<license>". check_plain($metadata['copyright']) ."</license>";
  $output .= '<trackList>';
  foreach ($items as $item) {
    $output .= '<track>';
    $output .= "<location>". check_plain($item['enclosure']['url']) ."</location>";
    $output .= "<creator>". check_plain($item['author']) ."</creator>";
    $output .= "<album>". check_plain($item['album']) ."</album>";
    $output .= "<title>". check_plain($item['title']) ."</title>";
    $annotation = $item['author'] ? $item['author'] ." - ". $item['title'] : $item['title'];
    $output .= "<annotation>". check_plain($annotation) ."</annotation>";
    $output .= "<duration>". check_plain($item['duration']) ."</duration>";
    $output .= "<image>". check_plain($item['image']['url']) ."</image>";
    $output .= "<info>". check_plain($item['link']) ."</info>";
    $output .= '</track>';
  }
  $output .= '  </trackList>';
  $output .= '</playlist>';

  // hopefully fixing 212913, i suspect nul's are making this choke
  // remove any nul characters
  $output = str_replace("\0", '', $output);

  drupal_set_header('Content-Type: text/xml; charset=utf-8');
  print $output;
  exit();
}

/**
 * Get the attached audio nodes.
 *
 * @param object $node
 *   The node whose audio nodes are to be retrieved.
 * @return
 *   An array containing all the audio nodes attached to a given node.
 */
function audio_feeds_children($node) {
  $children = array();
  
  // Find all nodereferences.
  $fields = content_fields(NULL, $node->type);
  foreach ($fields as $field_name => $field) {
    if ($field['type'] == 'nodereference' && is_array($node->{$field_name})) {
      foreach ($node->{$field_name} as $item) {
        // Keep only those references that are actually accessible and
        // downloadable audio nodes.
        if (($child = node_load($item['nid'])) && node_access('view', $child) && _audio_allow_play($child)) {
          $children[] = $child;
        }
      }
    }
  }
  
  return $children;
}