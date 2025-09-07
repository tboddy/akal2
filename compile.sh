rm -rf res/resources.o res/resources.h out/*
docker run --rm -v $PWD:/m68k -t registry.gitlab.com/doragasu/docker-sgdk:v2.00