# Arisu
A [SvenMod](https://github.com/sw1ft747/svenmod) simple plugin to change responses of cvars queries from servers

Using this plugin, any cvar you define in list.txt will return the value you set it to.

You can set any value with the exception only for '-1' that will return a bad query (non-existent cvar)

NOTE: This is not changing your cvar values in any way, only the query response is altered by it


## Example list.txt
```
example_cvar = 9999
example_cvar2 = -1
```

You can use console command 'arisu_reload' to reload the list file after editing it

## How to install

Extract the build zip file in the Sven Co-op directory
and add this line to your plugins.txt file

```
"Arisu" "1"
```

##

Thanks to [Sw1ft](https://github.com/sw1ft747) for [SvenMod](https://github.com/sw1ft747/svenmod)

