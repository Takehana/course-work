FROM ubuntu
RUN \
  apt-get update && \
  apt-get install -y python python-dev python-pip python-virtualenv && \
  rm -rf /var/lib/apt/lists/*

RUN pip install virtualenv
RUN virtualenv flask
RUN flask/bin/pip install flask
RUN flask/bin/pip install requests
COPY app.py .
CMD ["pwd"]
EXPOSE 8080
RUN chmod a+x app.py
CMD ["./app.py"]