#!/bin/bash

ps -e | grep 'UsoDisco' | awk '{printf $1}' | xargs kill -9