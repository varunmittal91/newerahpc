import os

import paas_errors

def checkPlatform():
   if os.name != "linux":
      paas_errors.setError(paas_errors.PAAS_ENOPLATFRM)
      return

   if os.system("modprobe aufs") == 0:
      import paas_mount_aufs as _mount
   elif os.system("modprobe overlayfs") == 0:
      import paas_mount_overlayfs as _mount
   else:
      paas_errors.setError(paas_errors.PAAS_ENOUMOUNT)
      return

   return 1
