# p3_empotrados

Para completar esta práctica he planteado la máquina expendedora como una máquina de estados, siendo cada parte de la funcionalidad un estado según sea necesario.

Toda la funcionalidad del arranque la he incluído en el setup del programa, es decir, escribir en el lcd CARGANDO... y mediante un bucle for, encender y apagar el led 1 con delays.

Antes de entrar a la funcionalidad del servicio cabe destacar que todas las mediciones de los sensores (temperatura, humedad, distancia y joystick) se hacen en threads separados.

En el servicio se encuentra la primera máquina de estados en la cual:

- Estado 0: escribe ESPERANDO CLIENTE en el lcd hasta que la distancia sea menor que 100, entonces pasa al siguiente estado, poniendo antes una marca de tiempo con millis.
- Estado 1: primero, comprueba la marca de tiempo comparándola con el tiempo actual, y si son menos de 5 segundos, mostrará la temperatura y humedad en el lcd. Si ya han pasado 5 segundos mostrará un elemento de la lista de cafés dependiendo del valor de una variable, esta variable cambia si la posición vertical del joystick es mayor que 950 o menor que 100. Si se pulsa el botón del joystick, se genera un número aleatorio entre 4000 y 8000, se pone una marca de tiempo y se pasa al estado 2.
- Estado 2: simplemente comprueba que la diferencia entre la marca de tiempo y el tiempo actual es mayor o igual al número aleatorio, si lo es, pasa al estado 3 y pone otra marca de tiempo, si no, muestra Preparando Cafe ... en el lcd y va aumentando la intensidad del led.
