<div align="center">
    <img width="400" align="center" src="./.github/pictures/preview.png">
    <h3 align="center">details</h3>
</div>
When piped, `details` will show the output from
the piped output into a GUI window. This makes it
easier to look at/for things such as logs, error
messages etc.

<br>

## Usage

### Example 1
```commandline
$ wine game.exe |& details
```

### Example 2
```commandline
$ details install.log
```

### Building
To install `details`, run `qmake` on the project's
root directory and run `make`.

### Installing
Run `bash ./install.sh`.

## License
MIT.

