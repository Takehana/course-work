FROM ubuntu
RUN apt-get update -y
RUN apt-get install -y python3-pip python-dev
ENV NAME asg1
COPY ./requirements.txt /app/requirements.txt
WORKDIR /app
RUN pip3 install -r requirements.txt
COPY . /app
ENTRYPOINT [ "python3" ]
CMD [ "app.py" ]
EXPOSE 8080