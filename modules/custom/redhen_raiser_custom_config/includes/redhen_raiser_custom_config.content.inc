<?php


/**
 * Helper for defining stubbed content and menu items.
 */
function raiser_sprint_stubbed_nodes() {

  // Optionally, add the nodes to create on install here.
  // Include a 'link_title' to create a menu link for a given node.
  // Note: if this array references menus that do not exist yet, they will be created in ts_install_helpers_crud_menu_items().
  // Note: if setting a 'parent_path' make sure that your menu item has the same 'menu_name.'

  $stubbed_nodes = array(
    'home' => array(
      'title' => 'Welcome to RedHen Raiser Sprint',
      'format' => 'rich_text',
      'weight' => 1,
      'body' => "Ethnic Odd Future salvia, sriracha keffiyeh art party craft beer polaroid normcore selfies chillwave sustainable semiotics Marfa letterpress. Chambray mumblecore food truck Shoreditch mustache actually. Drinking vinegar mixtape cliche, ethnic keytar stumptown Pitchfork retro Banksy mustache cardigan scenester Godard ethical. Leggings pour-over biodiesel yr chillwave quinoa plaid Portland banh mi selvage. Wolf flannel gentrify asymmetrical, Tonx authentic retro. Godard scenester Carles, PBR slow-carb gluten-free hella. Mumblecore biodiesel sustainable Carles letterpress YOLO.",
    ),
    // top-level menus:
    'About' => array(
      'title' => 'About RedHen Raiser',
      'link_title' => 'About',
      'menu_name' => 'main-menu',
      'format' => 'full_html',
      'weight' => 2,
    ),
    'Privacy' => array(
      'title' => 'Privacy Policy',
      'link_title' => 'Privacy',
      'menu_name' => 'main-menu',
      'format' => 'full_html',
      'weight' => 3,
    ),
    'Terms' => array(
      'title' => 'Terms & Conditions',
      'link_title' => 'Terms',
      'menu_name' => 'main-menu',
      'format' => 'full_html',
      'weight' => 4,
    ),
  );

  return $stubbed_nodes;
}

/**
 * Helper for creating stubbed contacts.
 *
 * @return array
 *   Array of sample contacts.
 */
function raiser_sprint_stubbed_contacts($number = 20) {
  $stubs = array();

  $default = array(
    'type' => 'redhen_contact',
    'bundle' => 'contact',
  );

  for ($i = 0; $i < $number; $i++) {
    $first_name = random_data_first_name();
    $middle_name = random_data_first_name();
    $last_name = random_data_last_name();
    $org = random_data_organization();
    $state = random_data_state();
    $city = random_data_city();
    $postal_code = random_data_postal_code();
    $street = random_data_street();
    $street2 = random_data_street2();

    $stubs[$i] = array();

    $stubs[$i]['properties'] = array(
      'redhen_state' => 1,
      'first_name' => $first_name,
      'middle_name' => $middle_name,
      'last_name' => $last_name,
    );

    $stubs[$i]['fields'] = array(
      'field_organization' => array(
        'value' => $org,
      ),
      'redhen_contact_email' => array(
        array(
          'value' => $first_name . '.' . $last_name . '@example.com',
          'default' => 1,
        ),
        array(
          'value' => $first_name . '@example.org',
        ),
      ),
      'field_billing_address' => array(
        'country' => 'US',
        'administrative_area' => $state,
        'locality' => $city,
        'postal_code' => $postal_code,
        'thoroughfare' => $street,
        'premise' => $street2,
      ),
    );

    $stubs[$i] = array_merge($default, $stubs[$i]);
  }

  return $stubs;
}

/**
 * Create users and link them to contacts. Updates contact entities.
 *
 * @param array $contacts
 *   Array of redhen_contact entities.
 */
function raiser_sprint_create_users_link_contacts(&$contacts) {
  /* @var $contact RedhenContact */
  foreach ($contacts as $contact) {
    // Create user.
    $edit = array();
    $edit['name'] = $contact->first_name . ' ' . $contact->last_name;
    $edit['mail'] = $contact->email();
    $edit['init'] = $edit['mail'];
    $edit['pass'] = $edit['name'];
    $edit['status'] = 1;

    $image_name = $contact->first_name . $contact->last_name . ".png";
    $image_destination = "public://$image_name";
    $edit['picture'] = ts_install_helpers_generate_image(200, 200, $image_name, $image_destination);

    $user = user_save('', $edit);

    // Link user to contact.
    if ($user) {
      $contact->uid = $user->uid;
      $contact->save();
    }
  }
}

/**
 * Helper for creating stubbed campaigns.
 *
 * @return array
 *   Array of sample campaigns.
 */
function raiser_sprint_stubbed_campaigns($number = 10) {
  $stubs = array();

  for ($i = 0; $i < $number; $i++) {
    $stubs[$i] = array(
      'type' => 'redhen_campaign',
      'bundle' => 'general',
    );

    $stubs[$i]['properties'] = array(
      'label' => random_data_campaign_name(),
      'redhen_state' => 1,
    );

    $stubs[$i]['fields'] = array(
      'redhen_campaign_donation_type' => array(
        'redhen_donation_type' => 'redhen_raiser_donation',
      ),
      'redhen_campaign_goal' => array(
        'value' => rand(100, 800000),
      ),
      'field_teaser' => array(
        'value' => devel_create_greeking(rand(3, 7)),
        'format' => 'rich_text',
      ),
      'field_start_date' => array(
        'value' => date('Y-m-d'),
      ),
      'field_end_date' => array(
        // 7-60 days from now.
        'value' => date('Y-m-d', time() + (rand(7, 60) * 24 * 60 * 60)),
      ),
    );

    // Story field.
    $stubs[$i]['fields']['field_story']['format'] = 'rich_text';
    $stubs[$i]['fields']['field_story']['value'] = '';
    $paragraphs = rand(2, 5);
    for ($p = 0; $p < $paragraphs; $p++) {
      $stubs[$i]['fields']['field_story']['value'] .= devel_create_greeking(rand(20, 100)) . "\n\n";
    }

    // Banner image field.
    $image_name = 'campaign-' . (string) rand(1, 999999) . '.png';
    $image_destination = "public://$image_name";
    $image_file = ts_install_helpers_generate_image(900, 400, $image_name, $image_destination);
    $stubs[$i]['fields']['field_banner_image'] = (array) $image_file;

    // Story media field.
    $image_name = 'campaign-' . (string) rand(1, 999999) . '.png';
    $image_destination = "public://$image_name";
    $image_file = ts_install_helpers_generate_image(600, 400, $image_name, $image_destination);
    $stubs[$i]['fields']['field_story_media'] = (array) $image_file;
  }

  return $stubs;
}

/**
 * Helper for creating stubbed campaign pages.
 *
 * @param array $campaigns
 *   Array of RedhenCampaign objects.
 * @param array $contacts
 *   Array of RedhenContact objects.
 * @param int $number
 *   How many campaign pages to create.
 * @param string $type
 *   Type of page to create. 'individual' or 'team'.
 * @param array $team_pages
 *   Team pages to reference, if making individual pages.
 *
 * @return array
 *   Array of campaign pages.
 */
function raiser_sprint_stubbed_campaign_pages($campaigns, $contacts, $number = 10, $type = 'individual', $team_pages = array()) {
  $stubs = array();

  for ($i = 0; $i < $number; $i++) {
    $stubs[$i] = array(
      'type' => 'redhen_campaign_page',
    );

    $stubs[$i]['properties']['redhen_state'] = 1;
    if ($type == 'team') {
      $stubs[$i]['bundle'] = 'team';
      $stubs[$i]['properties']['label'] = random_data_campaign_team_name();
      $stubs[$i]['properties']['campaign_page_type'] = 2;
    }
    if ($type == 'individual') {
      $stubs[$i]['bundle'] = 'individual';
      $stubs[$i]['properties']['label'] = random_data_first_name() . ' ' . random_data_last_name();
      $stubs[$i]['properties']['campaign_page_type'] = 1;
      if ($team_pages) {
        $team_page = random_data_array_item($team_pages);
        $stubs[$i]['properties']['team'] = $team_page->page_id;
      }
    }

    $contact = random_data_array_item($contacts);
    $stubs[$i]['properties']['author_uid'] = $contact->contact_id;

    $stubs[$i]['fields'] = array(
      'redhen_campaign_goal' => array(
        'value' => rand(50, 8000),
      ),
      'field_end_date' => array(
        // 7-60 days from now.
        'value' => date('Y-m-d', time() + (rand(7, 60) * 24 * 60 * 60)),
      ),
    );

    $campaign = random_data_array_item($campaigns);
    $stubs[$i]['properties']['campaign'] = $campaign->campaign_id;

    // Story field.
    $stubs[$i]['fields']['field_story']['format'] = 'rich_text';
    $stubs[$i]['fields']['field_story']['value'] = '';
    $stubs[$i]['fields']['field_story']['value'] .= devel_create_greeking(rand(20, 100));

    // Logo field.
    $image_name = 'campaign-page-' . (string) rand(1, 999999) . '.png';
    $image_destination = "public://$image_name";
    $image_file = ts_install_helpers_generate_image(200, 200, $image_name, $image_destination);
    $stubs[$i]['fields']['field_logo'] = (array) $image_file;

    // Story media field.
    $image_name = 'campaign-page-' . (string) rand(1, 999999) . '.png';
    $image_destination = "public://$image_name";
    $image_file = ts_install_helpers_generate_image(600, 400, $image_name, $image_destination);
    $stubs[$i]['fields']['field_story_media'] = (array) $image_file;
  }

  return $stubs;
}

/**
 * Create sample campaign updates.
 *
 * @param array $campaigns
 *   Array of RedhenCampaign objects.
 * @param array $team_campaign_pages
 *   Array of RedhenCampaignPage objects, team bundle.
 * @param array $individual_campaign_pages
 *   Array of RedhenCampaignPage objects, individual bundle.
 * @param int $number
 *   How many campaign updates to create.
 *
 * @return array
 *   Array of created nodes.
 */
function raiser_sprint_stubbed_campaign_updates($campaigns, $team_campaign_pages, $individual_campaign_pages, $number = 10) {
  $stubs = array();

  for ($i = 0; $i < $number; $i++) {
    $campaign = random_data_array_item($campaigns);

    $stubs[$i] = array(
      'type' => 'campaign_update',
      'body' => devel_create_para(rand(2, 5), 1),
      'format' => 'rich_text',
      'title' => devel_create_greeking(5, TRUE),
    );
    $stubs[$i]['custom_fields'] = array(
      'field_campaign_context' => array(
        'target_id' => $campaign->campaign_id,
      ),
    );

    // Sometimes leave the team context field empty.
    if (rand(0, 1)) {
      $team_campaign_page = random_data_array_item($team_campaign_pages);
      $stubs[$i]['custom_fields']['field_team_context']['target_id'] = $team_campaign_page->page_id;
    }

    // Use both individual and team pages. And sometimes leave it empty.
    $rand = rand(1, 3);
    if ($rand == 1) {
      // Use a random individual campaign page.
      $page_for_context = random_data_array_item($individual_campaign_pages);
      $stubs[$i]['custom_fields']['field_page_context']['target_id'] = $page_for_context->page_id;
    }
    if ($rand == 2) {
      // Use a random team campaign page.
      $page_for_context = random_data_array_item($team_campaign_pages);
      $stubs[$i]['custom_fields']['field_page_context']['target_id'] = $page_for_context->page_id;
    }
  }

  return $stubs;
}

/**
 * Create sample donations.
 *
 * @param array $contacts
 *   Array of RedhenContact objects, as people giving donations.
 * @param array $campaigns
 *   Array of RedhenCampaign objects, as recipients of donations.
 * @param int $number
 *   How many donations to create.
 * @param array $campaign_pages
 *   Array of RedhenCampaignPages to reference.
 */
function raiser_sprint_create_sample_donations($contacts, $campaigns, $number = 10, $campaign_pages = array()) {
  for ($i = 0; $i < $number; $i++) {
    $campaign = random_data_array_item($campaigns);
    $campaign_id = $campaign->campaign_id;
    $contact = random_data_array_item($contacts);
    // Apply half of the donations to campaign pages.
    if (count($campaign_pages) && rand(0, 1)) {
      $campaign_page = random_data_array_item($campaign_pages);
      $context_id = $campaign_page->page_id;
      $context_type = 'redhen_campaign_page';
    }
    else {
      $context_id = $campaign_id;
      $context_type = 'redhen_campaign';
    }
    $donation = new RedhenDonation(array(
      'type' => 'redhen_raiser_donation',
      'entity_type' => 'redhen_campaign',
      'entity_id' => $campaign_id,
      'author_uid' => $contact->uid,
      'contact_id' => $contact->contact_id,
      'status' => NULL,
      'transaction_entity_type' => NULL,
      'pledged' => NULL,
      'redhen_campaign_context_id' => $context_id,
      'redhen_campaign_context_type' => $context_type,
    ));
    raiser_sprint_process_sample_donation($donation);
  }
}

/**
 * Process single donation.
 *
 * @param RedhenDonation $donation
 *   Donation object to process, which simulates the donation form submission,
 *   including creating an order.
 */
function raiser_sprint_process_sample_donation(RedhenDonation $donation) {
  // Set query parameters, to provide expected values from
  // drupal_get_query_parameters() in redhen_campaign_form_redhen_donation_form_alter().
  if ($donation->redhen_campaign_context_type == 'redhen_campaign_page') {
    $_GET['campaign_page'] = $donation->redhen_campaign_context_id;
  }

  // Create $form array.
  $form_id = 'redhen_donation_form';
  $form = drupal_get_form($form_id, $donation);

  // Create $form_state array.
  $donation_settings = redhen_donation_entity_settings($donation->entity_type, $donation->entity_id);
  $contact = redhen_contact_load($donation->contact_id);
  $contact_email = $contact->email();
  $payment_method = 'commerce_payment_example|commerce_payment_commerce_payment_example';
  $form_state = array(
    'donation' => $donation,
    'donation_settings' => $donation_settings,
    'redhen_contact' => $contact,
    'build_info' => array(
      'args' => array(
        0 => $donation,
      ),
    ),
    'currency' => 'USD',
    'values' => array(
      'donation' => rand(25, 5000),
      'first_name' => $contact->first_name,
      'middle_name' => $contact->middle_name,
      'last_name' => $contact->last_name,
      'email' => $contact_email,
      'payment_method' => $payment_method,
      'product' => 1,
      'form_id' => 'redhen_donation_form',
      'op' => 'Donate',
      'field_donation_comment' => array(
        LANGUAGE_NONE => array(
          0 => array(
            'value' => devel_create_greeking(rand(5, 40)),
          ),
        ),
      ),
      'commerce_customer_address' => array(
        LANGUAGE_NONE => array(
          0 => array_merge(
            $contact->field_billing_address[LANGUAGE_NONE][0],
            array('element_key' => $payment_method)
          ),
        ),
      ),
      'payment_details' => array(
        'credit_card' => array(
          'number' => '4222222222222220',
          'exp_month' => '12',
          'exp_year' => '2019',
        ),
      ),
    ),
  );

  // Make the donation.
  drupal_prepare_form($form_id, $form, $form_state);
  redhen_donation_form_submit($form, $form_state);
}

/**
 * Add a sample payment rule for development.
 */
function raiser_sprint_add_sample_payment_method() {
  // Based on rules_ui_import_form_submit().
  $rules_export = '{ "commerce_payment_commerce_payment_example" : {
    "LABEL" : "Example payment",
    "PLUGIN" : "reaction rule",
    "OWNER" : "rules",
    "TAGS" : [ "Commerce Payment" ],
    "REQUIRES" : [ "commerce_payment" ],
    "ON" : { "commerce_payment_methods" : [] },
    "DO" : [
      { "commerce_payment_enable_commerce_payment_example" : {
          "commerce_order" : [ "commerce-order" ],
          "payment_method" : { "value" : { "method_id" : "commerce_payment_example" } }
        }
      }
    ]
  }
}';

  module_load_include('inc', 'rules', 'ui/ui.forms');
  $rules_config = rules_import($rules_export);

  if ($existing_config = rules_config_load($rules_config->name)) {
    // Copy DB id and remove the new indicator to overwrite the existing record.
    $rules_config->id = $existing_config->id;
    unset($rules_config->is_new);
  }

  $rules_config->save();
  $vars = array('@entity' => t('Rules configuration'), '%label' => $rules_config->label());
  watchdog('rules_config', 'Imported @entity %label.', $vars);
  drupal_set_message(t('Imported @entity %label.', $vars));
}
