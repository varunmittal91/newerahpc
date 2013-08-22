import paas_config
import paas_errors

def connectMysql(cmd_arguments):
   mysql_config = paas_config.getMysqlConfig(cmd_arguments)
   if not mysql_config:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      return
   return 1

   
