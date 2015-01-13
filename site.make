;======================================
; Charlotte's Web
; A sample implementation of
; RedHen Raiser Distribution
;
; by ThinkShout
;======================================
api = 2
core = 7.x

; Download the RedHen Raiser install profile and recursively build all its dependencies:
projects[redhen_raiser][type] = profile
projects[redhen_raiser][download][type] = git
projects[redhen_raiser][download][url] = "https://github.com/thinkshout/redhen_raiser.git"
projects[redhen_raiser][download][branch] = '7.x-1.x'


;======================================
; Use standard drush make syntax
; to customize your site. Take a
; close look at the redhen_raiser.make
; to avoid downloading duplicate or
; conflicting modules, libraries, or
; themes.
;
; These items will be placed in
; sites/all/. Make sure to include the
; [destination] as in the examples.
;
; A couple of recommended modules are
; included as examples: feel free to
; remove them.
;======================================

;======================================
; Libraries
;======================================

; ts_install_helpers
libraries[ts_install_helpers][download][type] = git
libraries[ts_install_helpers][download][url] = git@github.com:thinkshout/ts_install_helpers.git
libraries[ts_install_helpers][directory_name] = "ts_install_helpers"
libraries[ts_install_helpers][type] = "library"

;-----------------------------------------
; Themes
;-----------------------------------------
