#!/bin/bash

java -jar Ida.jar -a tests/*.ida
java -jar Ida.jar -t | less
