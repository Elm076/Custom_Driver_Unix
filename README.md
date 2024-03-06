Driver Desarrollado en Linux por Adrián González Pérez
Este driver consiste en crear un "espacio seguro" dentro del sistema de archivos en el cuál solo tendrá acceso los usuarios que
el desarrollador del driver considere oportunos.
Esto se hace mediante la gestión de los usuarios por el UID. Esto es igual de potente como de catastrófico, ya que si un atacante
descubriese cuáles son los UIDs permitidos por el driver podría intentar manipular su cuenta o otras cuentas (aunque necesitaría
una escalada de privilegios muy probablemente) para poder infiltrarse en el espacio seguro.
Se definen todos los parámetros necesarios para crear el dispositivo y su clase (o más dispositivos si se quiere) y se definen las operaciones
básicas del driver que son:
  open
  read
  write
  release
Además de las necesarias para inicializarlo en el sistema y de borrarlo.

El driver es bastante básico en sí en su funcionamiento, esto es debido a la falta de tiempo que he tenido para realizarlo.
Se podría implementar para un mejor funcionamiento un sistema de cifrador por contraseña o además del UID alguna otra autentificación
pero es algo complejo que actualmente se escapa de mi alcance y empeoraría el rendimiento considerablemente si se usará muy ocurrentemente
(otra posible forma de atacar el equipo).
