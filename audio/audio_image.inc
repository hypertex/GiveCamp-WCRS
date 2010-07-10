<?php
// $Id: audio_image.inc,v 1.10 2008/10/02 18:03:44 drewish Exp $

/**
 * Audio Image Functions.
 */

/**
 * Returns an array of all the different image types with "clean" descriptions.
 *
 * @param $index
 *   Optional index into the array.
 * @return
 *   If $index is specified a string will be returned. If $index isn't
 *   specified an array of strings will be returned.
 */
function audio_image_type_clean_array($index = NULL) {
  $ret = array(
    0x00 => 'other',
    0x01 => 'file_icon',
    0x02 => 'other_file_icon',
    0x03 => 'cover_front',
    0x04 => 'cover_back',
    0x05 => 'leaflet',
    0x06 => 'media',
    0x07 => 'lead_artist',
    0x08 => 'artist',
    0x09 => 'conductor',
    0x0A => 'band_orch',
    0x0B => 'comp',
    0x0C => 'lyricist',
    0x0D => 'rec_loc',
    0x0E => 'recording',
    0x0F => 'performance',
    0x10 => 'screen_cap',
    0x11 => 'fish',
    0x12 => 'illust',
    0x13 => 'band_logo',
    0x14 => 'pub_logo'
  );
  return is_null($index) ? $ret : $ret[$index];
}

/**
 * Returns an array of all the different image types with "dirty" descriptions.
 *
 * @param $index
 *   Optional index into the array.
 * @return
 *   If $index is specified a string will be returned. If $index isn't
 *   specified an array of strings will be returned.
 */
function audio_image_type_dirty_array($index = NULL) {
  $ret = array(
    0x00 => t('Other'),
    0x01 => t("32x32 pixels 'file icon' (PNG only)"),
    0x02 => t('Other file icon'),
    0x03 => t('Cover (front)'),
    0x04 => t('Cover (back)'),
    0x05 => t('Leaflet page'),
    0x06 => t('Media (e.g. label side of CD)'),
    0x07 => t('Lead artist/lead performer/soloist'),
    0x08 => t('Artist/performer'),
    0x09 => t('Conductor'),
    0x0A => t('Band/Orchestra'),
    0x0B => t('Composer'),
    0x0C => t('Lyricist/text writer'),
    0x0D => t('Recording Location'),
    0x0E => t('During recording'),
    0x0F => t('During performance'),
    0x10 => t('Movie/video screen capture'),
    0x11 => t('A bright coloured fish'),
    0x12 => t('Illustration'),
    0x13 => t('Band/artist logotype'),
    0x14 => t('Publisher/Studio logotype'),
  );
  return is_null($index) ? $ret : $ret[$index];
}

/**
 * Creates a temporary audio image from a variable.
 *
 * The image is cropped to a square and then resized to the image size setting.
 *
 * @param $basename
 *   Name of the audio file this image accompanies.
 * @param $data
 *   The raw image data.
 * @param $mimetype
 *   The MIME type of the image.
 * @param $pictype
 *   Integer pictype indexes from audio_image_type_clean_array() or
 *   audio_image_type_dirty_array().
 * @return
 *   A file object with image info or FALSE on error.
 */
function audio_image_save_data($basename, $data, $mimetype, $pictype) {
  global $user;

  // Gotta have a name to save to.
  $filepath = _audio_image_filename($basename, $mimetype, $pictype, TRUE);
  if (!$filepath) {
    return FALSE;
  }

  // Save the data.
  $filepath = file_save_data($data, $filepath, FILE_EXISTS_RENAME);
  if (!$filepath) {
    return FALSE;
  }

  // Make sure it's a valid image.
  $image = image_get_info($filepath);
  if (!$image) {
    file_delete($filepath);
    return FALSE;
  }

  // Resize the image
  $size = variable_get('audio_image_size', 170);
  if (image_scale_and_crop($filepath, $filepath, $size, $size)) {
    // Changing the image dimensions will affect the file size. Clear out
    // PHP's cached value so we can find the new size.
    clearstatcache();
    $image = image_get_info($filepath);
  }

  // Store the file in the database so it can be removed by cron if it's not
  // used.
  $file = new stdClass();
  $file->filepath = $filepath;
  $file->filename = basename($file->filepath);
  $file->filemime = $mimetype;
  $file->filesize = $image['file_size'];
  $file->uid = $user->uid;
  $file->status = FILE_STATUS_TEMPORARY;
  $file->timestamp = time();

  drupal_write_record('files', $file);

  $file->pictype = $pictype;
  $file->height = $image['height'];
  $file->width = $image['width'];

  return $file;
}

/**
 * If the file is an image it will be resized to meet the audio image size
 * guidelines.
 *
 * @param $file
 *   A Drupal file object. This function may resize the file affecting its size.
 * @return
 *   An array. If the file is an image and did not meet the requirements, it
 *   will contain an error message.
 */
function audio_image_validate_size($file) {
  $errors = array();

  // Check first that the file is an image.
  if ($info = image_get_info($file->filepath)) {
    $size = variable_get('audio_image_size', 170);
    if ($info['width'] > $size || $info['height'] > $size) {
      // Try to resize the image to fit the dimensions.
      if (image_get_toolkit() && image_scale_and_crop($file->filepath, $file->filepath, $size, $size)) {
        drupal_set_message(t('The image was resized to fit within the maximum allowed dimensions of %height x %width pixels.', array('%height' => $size, '%width' => $size)));

        // Clear the cached filesize and refresh the image information.
        clearstatcache();
        $info = image_get_info($file->filepath);
        $file->filesize = $info['file_size'];
      }
      else {
        $errors[] = t('The image is too large; the maximum dimensions are %dimensions pixels.', array('%dimensions' => $maximum_dimensions));
      }
    }
  }

  return $errors;
}


/**
 * Creates the image's filename in the form directory/prefix_imagetype.ext
 *
 * @param $prefix
 *   The prefix of the filename, probably should be the nid.
 * @param $mimetype
 *   The image's mime type. jpeg, png and gif are the only formats allowed.
 * @param $pictype
 *   Integer specifying the picture type.
 * @param $in_tempdir
 *   Boolean indicating if the file be in the temp directory.
 * @return
 *   Full filepath or null in case of an error.
 */
function _audio_image_filename($prefix, $mimetype, $pictype = 0x03, $in_tempdir = FALSE) {
  $directory = $in_tempdir ? file_directory_temp() : audio_get_directory() . '/images';
  file_check_directory($directory, TRUE);

  //get the clean image type
  $image_type = audio_image_type_clean_array($pictype);

  switch ($mimetype) {
    case 'image/jpeg':
      $ext = '.jpg';
      break;
    case 'image/png':
      $ext = '.png';
      break;
    case 'image/gif':
      $ext = '.gif';
      break;
    default:
      // don't return a filename for any other extensions
      return;
  }

  // combine the path and file extension from the mimetype
  return $directory .'/'. $prefix .'_'. $image_type . $ext;
}
