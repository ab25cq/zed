#!/bin/sh

export EDITOR=vin
git config --global user.email ab25cq@icloud.com
git config --global user.name ab25cq

git add .
git commit 
git remote set-url origin git@github.com:ab25cq/zed.git
git push -f -u origin master

