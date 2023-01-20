#!/bin/sh
echo 0.1.$(git rev-parse --abbrev-ref HEAD).$(git rev-parse --verify HEAD)-$(date +%F)