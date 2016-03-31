#!/bin/bash
# inside this git repo we'll pretend to be a new user
git config user.name "Travis CI build bot"
git config user.email "caesaria@travis.ci"

# The first and only commit to this new Git repo contains all the
# files present with the commit message "Deploy to GitHub Pages".

mv bin/caesaria.exe bin/caesaria-release.exe
git add bin/caesaria-release.exe
git commit -m "deploy win release binary to repository"

# Force push from the current repo's master branch to the remote
# repo's gh-pages branch. (All previous history on the gh-pages branch
# will be lost, since we are overwriting it.) We redirect any output to
# /dev/null to hide any sensitive credential data that might otherwise be exposed.
git push --force --quiet "https://${GH_TOKEN}@${GH_REF}" master > /dev/null 2>&1
