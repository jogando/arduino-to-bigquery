# arduino-to-bigquery

## Deploy GCP infrastructure

We need to have a *GCS Bucket* for storing the *Terraform state*.

Example:
```
gsutil mb gs://name-of-my-tf-state-bucket
gsutil versioning set on gs://name-of-my-tf-state-bucket
```

Modify the `terraform.tfvars` file with your own settings.

```
cd iac
terraform init -backend-config="bucket=name-of-my-tf-state-bucket"
terraform plan -out tfplan
terraform apply tfplan
```

## Arduino

### Generating the key pair

We need to generate a pair of public/private keys for authenticating our *Arduino* with *GCP IoT Core*

```
cd arduino/key-gen
python3 -m venv env
source env/bin/activate
pip3 install -r requirements.txt
python3 main.py
```

This will generate something like this:

```
-----BEGIN EC PRIVATE KEY-----
MHcCAQEEIMpZzcomTqSTn9FHizBjaPQ12jN+IycK15fgXKwooETtoAoGCCqGSM49
AwEHoUQDQgAEYnzRg8vpFIsB6foqJvEBrO2TDE7ZHVs0lGQ+acH9e59+YUSHO35e
HVX935r+lTWIWbUBYYzHl4ZgUZWjdmzy3Q==
-----END EC PRIVATE KEY-----

Device key:
 0xca,0x59,0xcd,0xca,0x26,0x4e,0xa4,0x93,0x9f,0xd1,0x47,0x8b,0x30,0x63,0x68,0xf4,0x35,0xda,0x33,0x7e,0x23,0x27,0xa,0xd7,0x97,0xe0,0x5c,0xac,0x28,0xa0,0x44,0xed

-----BEGIN PUBLIC KEY-----
MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEYnzRg8vpFIsB6foqJvEBrO2TDE7Z
HVs0lGQ+acH9e59+YUSHO35eHVX935r+lTWIWbUBYYzHl4ZgUZWjdmzy3Q==
-----END PUBLIC KEY-----
```

We will need to create a file called `greenhouse.pub` with the contents of the *public key* generated by the script. This will be used for registering the device in *IoT Core*

The *Device key* section generated by the script will need to be stored in the `ciotc_config.h` file:

```
const unsigned char private_key[] = { 0xca,0x59,0xcd,0xca,0x26,0x4e,0xa4,0x93,0x9f,0xd1,0x47,0x8b,0x30,0x63,0x68,0xf4,0x35,0xda,0x33,0x7e,0x23,0x27,0xa,0xd7,0x97,0xe0,0x5c,0xac,0x28,0xa0,0x44,0xed };
```

### Registering our device with IoT Core

Now that we have *IoT Core* and our *public/private key pair* created, we can register our `greenhouse` device

```
REGISTRY=cloudiot-registry 
DEVICE=greenhouse
PROJECT=your-project-name-goes-here
REGION=us-central1
gcloud iot devices create $DEVICE --region=$REGION \
    --registry=$REGISTRY \
    --project=$PROJECT \
    --public-key path=greenhouse.pub,type=es256
```

You should get a message `Created device [greenhouse].` and be able to see the device in *GCP*.

### Configuring the device - ciotc_config.h

You need to update these variables with your own values

```
// Wifi
const char* ssid = "your-ssid-goes-here";
const char* password = "your-password-goes-here";

// Cloud iot details.
const char* project_id = "your-gcp-project-goes-here";
const char* location = "us-central1";
const char* registry_id = "cloudiot-registry";
const char* device_id = "greenhouse";

 [...]

// Needs to be generated!
const unsigned char private_key[] = { 0xca,0x59,0xcd,0xca,0x26,0x4e,0xa4,0x93,0x9f,0xd1,0x47,0x8b,0x30,0x63,0x68,0xf4,0x35,0xda,0x33,0x7e,0x23,0x27,0xa,0xd7,0x97,0xe0,0x5c,0xac,0x28,0xa0,0x44,0xed };
```

### Running the Arduino code

You should see something like this (depending the log level you have configured in your Arduino)

```
Connecting to WiFi
Waiting on time sync...
Connecting...
eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE2NzMwMTY0NDYsImV4cCI6MTY3MzAyMDA0NiwiYXVkIjoianVsaWFub2dhbmRvLXNhbmRib3gifQ.tt7ecaQwbRmhIEI_WJ6BsHiLLFBlU6iX5Nhtq6QbfqrPNk7TffW7qYrMQu85CwD7QOm6tYwTUK61oTaecaAnAA
connected

Library connected!
Incoming Topic: /devices/greenhouse/config
Outcoming: subtopic: /greenhouse/temperature,data: 21
Outcoming: subtopic: /greenhouse/humidity,data: 22
```