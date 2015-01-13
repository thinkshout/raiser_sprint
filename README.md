# Capital Area Food Bank DC - Campaign 

Run:

`scripts/build.sh ../drupal` to build the complete site in a sibling "drupal"
directory.

`scripts/deploy.sh` to deploy the site to its destination host, such as Pantheon
or Acquia.

## Installation

* `drush si redhen_raiser --db-url=""`
* To install test content, run `drush eval "redhen_raiser_custom_default_content()"`. There is currently a bug where this cannot be run in a single step along with the install process.