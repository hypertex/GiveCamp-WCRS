<?php
// $Id: audio_images.admin.inc,v 1.1 2008/05/26 17:26:13 drewish Exp $

/**
 * Module's settings form.
 */
function audio_images_admin_settings() {
  $form['audio_image_size'] = array(
    '#type' => 'select',
    '#title' => t('Image size'),
    '#default_value' => variable_get('audio_image_size', 170),
    '#options' => array(
      '170' => '170 X 170',
      '150' => '150 X 150',
      '125' => '125 X 125',
      '100' => '100 X 100',
      '75'  => '75 X 75',
    ),
    '#description' => t('The size (in pixels) that the theme will display the images. Default is <em>170x170</em>.'),
  );
  $form['audio_default_image_type'] = array(
    '#type' => 'select',
    '#title' => t('Default image type'),
    '#default_value' => variable_get('audio_default_image_type', 0x03),
    '#options' => audio_image_type_dirty_array(),
    '#description' => t("Select the image type you would like displayed. <strong>NOTE:</strong> If an audio node does not have an image of this type associated with it, a random image will be chosen.")
  );

  return system_settings_form($form);
}

