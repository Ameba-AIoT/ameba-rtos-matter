# Matter Certification

For a device to achieve Matter certification, it must undergo a rigorous process involving test events organized by the Connectivity Standards Alliance (CSA). The process ensures that the product meets all necessary specifications and requirements.

## Introduction to Matter Test Harness (TH)

The Matter Test Harness (TH) is a critical tool used during the Matter certification process, typically running on a Raspberry Pi. It provides a user-friendly interface for selecting and executing test cases required for certification. There are several modes of operation for executing test cases:

1. **UI-Automated:** Fully automated execution of test cases by the TH.
2. **UI-Semi-Automated:** Partially automated, where some steps require manual intervention using commands on the Chip-Tool, with results submitted via the TH.
3. **UI-Manual:** Completely manual operation where all steps in the test case are manually executed on the Chip-Tool, and results are submitted through the TH.
4. **Verification Steps Document:** Test cases not included in the TH follow specific steps outlined in the Verification Steps Document.

### Test Harness (TH) Installation

Make sure you have read through the guides in [certification-tool](https://github.com/project-chip/certification-tool/blob/main/docs/Matter_TH_User_Guide/Matter_TH_User_Guide.adoc) to better understand which TH version is required for your Certification.

Instructions for updating and using the TH are also detailed in the Matter TH User Guide.

### PAA Certificate

When using a newly generated PAA (Product Attestation Authority), the TH must include the `--paa-trust-store-path` argument for device pairing:

**Step 1: Update PAA certificates on TH and set CHIP_TOOL_USE_PAA_CERTS to true.**

**Step 2: Copy new PAA certificate to `/var/paa-root-certs/`**

**Step 3: Run any automated chip-tool tests and it will use the `--paa-trust-store-path` argument.**

### PICS and PICS Tools

The PICS files indicate the Matter features that your device supports and determine the test cases to be tested in Matter Certification Test. The PICS Tool is also available to assist in the selection and validation of XML PICS files.

The reference XML PICS files include all the PICS files of one or several cluster. Upload the XML PICS files to PICS Tool and start selecting the features supported by your device. Click the button Validate All on the PICS Tool and a list of Test Cases that must be tested will be generated.
