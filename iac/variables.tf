variable "project_name" {
  type = string
  description = "Name of the GCP project in which we will be deploying all of our resources"
}

variable "region" {
  type = string
  description = "Name of the region in which we will create our GCP resources"
  default = "us-central1"
}

variable "bigquery_location" {
  type = string
  description = "Name of the location in which we will create our Bigquery dataset"
  default = "US"
}