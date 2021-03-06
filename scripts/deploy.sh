#!/bin/bash
set -e

#
# Calls build.sh and git clone on the specified hosting repository;
# uses git to gather changes from host repository in the current build;
# adds changes to host's git repository
#
# Usage: scripts/deploy.sh <HOST_TYPE> <SSH_ADDRESS_OF_REPO> from the
# profile main directory. <HOST_TYPE> can be "pantheon", "acquia", or
# "generic"
#

ORIGIN=$(pwd)
SKIP=$1
source scripts/config.sh
HOSTDRUPALROOT=$HOSTTYPE
source scripts/hosttypes/$HOSTTYPE.sh

if [[ -z $MULTIDEV ]]; then
  MULTIDEV=false
fi

usage()
{
cat << EOF
usage: $0 options

deploy.sh <# of commits from made directly on host since last push
Be sure to put your configuration info into scripts/config.sh

OPTIONS:
  -h      Show this message
  -b      Branch to checkout (optional)

EOF
}
  #-r      Not Used

confirmpush () {
  echo "Git add & commit completed. Ready to push to Repo at $GITREPO."
  read -r -p "Push? [y/n] " response
  case $response in
    [yY][eE][sS]|[yY])
      true
      ;;
    *)
      false
      ;;
  esac
}

confirmcommitmsg () {
  echo $'\n'
  echo "Review commit message."
  echo $'\n'
  cat $TEMP_BUILD/commitmessage
  echo $'\n'
  read -r -p "Commit message is correct? [y/n] " response
  case $response in
    [yY][eE][sS]|[yY])
      true
      ;;
    *)
      false
      ;;
  esac
}

librariescheck () {
  git diff --name-status $TEMP_BUILD/$HOSTDRUPALROOT/profiles/$PROJECT/libraries >> $TEMP_BUILD/librariesdiff #--diff-filter=ACMRTUXB
  WORDCOUNT=`cat $TEMP_BUILD/librariesdiff | wc -l`
  if [ $WORDCOUNT -gt 0 ]; then
    echo $'\n'
    echo 'Library files changed:'
    echo $'\n'
    cat $TEMP_BUILD/librariesdiff
    echo $'\n'
    read -r -p "Are you sure you want to update libraries? [y/n] " response
    case $response in
      [yY][eE][sS]|[yY])
        true
        ;;
      *)
        false
        ;;
    esac
fi
}

versioncheck () {
  echo $'\n'
  read -r -p "Did you check your versions? [y/n] " response
  case $response in
    [yY][eE][sS]|[yY])
      true
      ;;
    *)
      false
      ;;
  esac
}

BRANCH=
COMMITTO=
COMMITFROM=
while getopts “h:b:r:t:f:” OPTION
do
  case $OPTION in
    h)
      usage
      exit 1
      ;;
    b)
      BRANCH=$OPTARG
      ;;
    t)
      COMMITTO=$OPTARG
      ;;
    f)
      COMMITFROM=$OPTARG
      ;;
    ?)
       usage
       exit 1
       ;;
     esac
done

# Branch is optional, but here is where we would check for non-optional args
# if [[ -z $BRANCH ]] || [[ -z $SERVER ]]
# then
#      usage
#      exit 1
# fi

if [[ -z $GITREPO ]]; then
  echo 'Set $GITREPO in scripts/config.sh'
  exit 1
fi

if [ "x$HOSTTYPE" == "x" ]; then
  usage
fi

if [ "x$SKIP" == "x" ]; then
  SKIP=0
fi

case $OSTYPE in
  darwin*)
    TEMP_BUILD=`mktemp -d -t tmpbuild`
    ;;
  *)
    TEMP_BUILD=`mktemp -d`
    ;;
esac

# Clone the hosting Repo first, as a bad argument can cause this to fail
echo "Cloning $HOSTTYPE Git Repo..."

# If host allows for multidev, check if a branch is specified
if [[ $MULTIDEV == "true" ]]; then
  if [[ -z $BRANCH ]]; then
    git clone $GITREPO $TEMP_BUILD/$HOSTTYPE
  else
    echo "Checkout $BRANCH branch..."
    git clone --branch $BRANCH $GITREPO $TEMP_BUILD/$HOSTTYPE
  fi
else
  git clone $GITREPO $TEMP_BUILD/$HOSTTYPE
fi

echo "$HOSTTYPE Clone complete, calling build.sh -y $TEMP_BUILD/drupal..."
if [[ -z $COMMITTO ]]; then
  echo "DEPLOY FROM MASTER"
  scripts/build.sh -y $TEMP_BUILD/drupal
else
  echo "DEPLOY UNTIL COMMIT $COMMITTO"
  # Checkout commit requested.
  git checkout $COMMITTO
  GETDEPLOYCOMMIT=`git rev-list $COMMITTO --timestamp --max-count=1`
  FILTER=" *"
  DEPLOYCOMMITDATEUNIX=${GETDEPLOYCOMMIT%%$FILTER}
  DEPLOYCOMMITDATE=`date -r $DEPLOYCOMMITDATEUNIX '+%m/%d/%Y %H:%M:%S'`
  scripts/build.sh -y $TEMP_BUILD/drupal
  # Ensure we are back to master
  git checkout master
fi

# Remove the scripts folder for security purposes:
rm -rf $TEMP_BUILD/drupal/sites/all/scripts
# due to issue 1875510 on d.o, we have to hack the profile if we are on drush 5.8:
rm -rf $TEMP_BUILD/drupal/profiles/$PROJECT/.git
rm -rf $TEMP_BUILD/drupal/profiles/$PROJECT/modules/contrib/*/.git
rm -rf $TEMP_BUILD/drupal/profiles/$PROJECT/modules/contrib/*/.gitignore
rm -rf $TEMP_BUILD/drupal/profiles/$PROJECT/libraries/*/.git
rm -rf $TEMP_BUILD/drupal/sites/all/libraries/*/.git
# Get rid of meddling .gitignore files:
rm $TEMP_BUILD/drupal/profiles/$PROJECT/.gitignore
rm $TEMP_BUILD/drupal/sites/all/.gitignore
# Strip out weird extra libraries files in /1/
echo "removing 1/libraries"
rm -rf $TEMP_BUILD/drupal/profiles/$PROJECT/1/
# Protect our remote .git, then blow away our remote Drupal root and replace it with our new one.
# We have to do this because sometime the remote .git is in the Drupal root (Pantheon) and sometimes
# it isn't (Acquia)
mv $TEMP_BUILD/$HOSTTYPE/.git $TEMP_BUILD/
rm -rf $TEMP_BUILD/$HOSTDRUPALROOT
mv $TEMP_BUILD/drupal $TEMP_BUILD/$HOSTDRUPALROOT
# Put our .git back where it belongs:
rm -rf $TEMP_BUILD/$HOSTTYPE/.git
mv $TEMP_BUILD/.git $TEMP_BUILD/$HOSTTYPE/

echo "mv $TEMP_BUILD/$HOSTTYPE/.git $TEMP_BUILD/
rm -rf $TEMP_BUILD/$HOSTDRUPALROOT
mv $TEMP_BUILD/drupal $TEMP_BUILD/$HOSTDRUPALROOT
rm -rf $TEMP_BUILD/$HOSTTYPE/.git
mv $TEMP_BUILD/.git $TEMP_BUILD/$HOSTTYPE/"

# Now let's build our commit message.
# git plumbing functions don't attend properly to --exec-path
# so we end up jumping around the directory structure to make git calls
# First, get the last hosting repo commit date so we know where to start
# our amalgamated commit comments from:
if [[ -z $COMMITFROM ]]; then
  cd $TEMP_BUILD/$HOSTTYPE
  COMMIT=`git rev-list --all --timestamp --max-count=1 --skip=$SKIP`
  cd $ORIGIN
else
  cd $ORIGIN
  COMMIT=`git rev-list $COMMITFROM --timestamp --max-count=1`
fi
FILTER=" *"
COMMITDATEUNIX=${COMMIT%%$FILTER}
COMMITDATE=`date -r $COMMITDATEUNIX '+%m/%d/%Y %H:%M:%S'`

# Git log for commit message
cd $ORIGIN
URLINFO=`cat .git/config | grep url`
BUILDREPO=${URLINFO##*@}

# Now we start building the commit message
echo "Commit generated by ThinkShout's deploy.sh script." > $TEMP_BUILD/commitmessage
echo "Amalgamating the following commits from $BUILDREPO:" >> $TEMP_BUILD/commitmessage

if [[ -z $COMMITTO ]]; then
  echo "Amalgamating commit comments since: $COMMITDATE"
  git log --pretty=format:"%h %s" --since="$COMMITDATE" >> $TEMP_BUILD/commitmessage
else
    echo "Amalgamating commit comments after-$COMMITDATE and before-$DEPLOYCOMMITDATEUNIX"
  git log --pretty=format:"%h %s" --after="$COMMITDATE" --before="$COMMITTO" >> $TEMP_BUILD/commitmessage
fi

if [[ -z $EDITOR ]]; then
  echo "Running vi to customize commit message: close editor to continue script."
  vi $TEMP_BUILD/commitmessage
else
  echo "Running $EDITOR to customize commit message: close editor to continue script."
  $EDITOR $TEMP_BUILD/commitmessage
fi

cd $TEMP_BUILD/$HOSTTYPE

if confirmcommitmsg; then
echo "Commit message approved."
else
echo "Commit message not approved."
  exit 1
fi

if versioncheck; then
echo "Versioning approved."
else
echo "Versioning not approved."
  exit 1
fi

if librariescheck; then
  echo $'\n'
else
  echo "Libraries updates not approved."
  exit 1
fi

echo "Writing git ls-files -mo to $TEMP_BUILD/changes"
# Checkout files that we don't want removed from the host, like settings.php.
# This function should be defined in the host include script.
protectfiles;

git ls-files -d --exclude-standard > $TEMP_BUILD/deletes
echo "Adding file deletions to GIT"
while read LINE
  do
    echo "Deleted: $LINE";
    git rm $LINE;
done < $TEMP_BUILD/deletes

git ls-files -mo --exclude-standard > $TEMP_BUILD/changes
echo "Adding new and changed files to GIT"
while read LINE
  do
    echo "Adding $LINE";
    git add "$LINE";
done < $TEMP_BUILD/changes
git status
echo "Committing changes"
git commit --file=$TEMP_BUILD/commitmessage >> /dev/null
git log --max-count=1
echo $'\n'
if confirmpush; then
  git push
else
  echo "Changes have not been pushed to Git Repository at $GITREPO."
  echo "To push changes:"
  echo "> cd $TEMP_BUILD/$HOSTTYPE"
  echo "> git push"
fi
echo "Build script complete. Clean up temp files with:"
echo "rm -rf $TEMP_BUILD"
cd $ORIGIN
