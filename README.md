# Installation profile to build RedHen Raiser for ThinkShout's code sprint.

## Installation

* `scripts/build.sh ../drupal` to build the complete site in a sibling "drupal"
directory.
* `drush si redhen_raiser --db-url=""`
* To install test content, run `drush eval "redhen_raiser_custom_default_content()"`. There is currently a bug where this cannot be run in a single step along with the install process.

## Deployment:

* `scripts/deploy.sh` to deploy the site to Pantheon.