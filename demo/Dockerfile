FROM node:lts-alpine

COPY package.json yarn.lock /app/

WORKDIR /app

RUN yarn install

COPY docker.js server.js /app/

EXPOSE 3000

CMD ["yarn", "start"]
