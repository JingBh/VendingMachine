FROM node:lts as builder-webui

COPY web/package.json web/yarn.lock /app/

WORKDIR /app

RUN yarn install

COPY web/src /app/src
COPY web/index.html web/postcss.config.js web/tailwind.config.js web/vite.config.js /app/

RUN yarn build

FROM alpine:latest as builder

RUN apk add --no-cache g++ cmake make

COPY lib /app/lib
COPY src /app/src
COPY CMakeLists.txt /app/

WORKDIR /app

COPY --from=builder-webui /app/dist/include /app/web/dist/include

RUN cmake -DCMAKE_BUILD_TYPE=Release -DVENDINGMACHINE_DOCKER=ON -DVENDINGMACHINE_BUILD_TESTS=OFF . && make

FROM alpine:latest

RUN apk add --no-cache libstdc++

COPY --from=builder /app/VendingMachine /app/VendingMachine

WORKDIR /app

EXPOSE 8206

CMD ["./VendingMachine"]
