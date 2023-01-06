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