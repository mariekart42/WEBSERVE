# Use a lightweight web server as the base image
FROM nginx:alpine

# Remove the default NGINX configuration
RUN rm /etc/nginx/conf.d/default.conf

# Copy your custom NGINX configuration
COPY my-nginx.conf /etc/nginx/conf.d/default.conf

# Copy your HTML file to the default NGINX directory
COPY read.html /usr/share/nginx/html

# Expose port 80 to allow incoming connections
EXPOSE 80