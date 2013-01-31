Gem::Specification.new do |s|
    s.name = 'fsearch'
    s.version = '0.0.1'
    s.date = '2013-01-31'
    s.summary = 'FSearch is a library that searches the filesystem for files matching a pattern.'
    s.description = "Searches the filesystem for files matching a pattern.\nFlags to exclude directores, require the files to match one of many patterns, or exclude files matching a certain pattern.\nUsing native APIs on Linux / OSX to be able to do so quickly."
    s.authors = ['Rickard Bäckman']
    s.files = Dir.glob('lib/**/*.rb') +
        Dir.glob('ext/**/*.{c,h,rb}')
    s.extensions = ['ext/fsearch/extconf.rb']
    s.executables = []
end
