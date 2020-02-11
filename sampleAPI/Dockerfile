FROM node:12-alpine

WORKDIR /opt/sampleAPI
COPY *.json ./
RUN npm install

COPY *.js ./
EXPOSE 8080/tcp
CMD ["index.js"]
