#!/usr/bin/env bash
set -Eeuo pipefail

# Run nCompass angular build using Docker.
#
echo "ANGULAR_SRC =  $ANGULAR_SRC"

#if [ -f 'Dockerfile']

	nodeImg="ncompass:angular-$BUILD_NUMBER"
	docker build -t "$nodeImg" "$ANGULAR_SRC"

	echo 'extracting files from containers:'
	echo -n $'\tweb...'
	docker run "$nodeImg" tar --create --directory //artifacts/ . | tar --extract --directory "$ANGULAR_ARTIFACTS"

	docker run --rm --mount "type=bind,src=$ANGULAR_TEST_ARTIFACTS,dst=/angular-test-host" "$nodeImg" \
		bash -c "npm run test:prod:docker; \
		cd /app/coverage/ncompass-ui; \
		tar -czf /angular-test-host/angular-coverage.tgz *; \
		cp /app/test/ncompass-ui/* /angular-test-host/"
echo 'done'
#fi
