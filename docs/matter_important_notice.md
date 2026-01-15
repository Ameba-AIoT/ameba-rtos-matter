# Important Notice

This page is where all important announcements and updates related to the project will be posted.

## all-clusters-app.zap location change
If you need to change all-clusters-app.zap, please change it in the following directory: `example/chiptest/all-clusters-app.zap`

This is because we have added all files required for all supported clusters in chip main makefile of all-clusters but the default all-clusters-app.zap in connectedhomeip does not include some clusters supported by Ameba. Therefore, build errors occur.

The only solution is to add Ameba's all-clusters-app.zap.

In `project/amebaXXX/Makefile`, the default all-clusters-app.zap in connectedhomeip will be replaced by Ameba's all-clusters-app.zap

    cp $(MATTER_EXAMPLEDIR)/chiptest/all-clusters-app.zap $(ALL_CLUSTERS_ZAP)
