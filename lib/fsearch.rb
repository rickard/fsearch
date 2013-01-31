require 'fsearch/fsearch'

module FSearch
    def self.search dir, pattern, opts
        search = FSearch::Search.new(dir)
        return search.search(dir, pattern, opts)
    end

    class Search
        def initialize dir
            @dir = dir
            @only_fp = []
            @exclude_dp = []
            @exclude_fp = []
        end

        def search dir, pattern, opts
            if opts.has_key?(:exclude_file_patterns)
                patterns = opts[:exclude_file_patterns]
                raise TypeError, 'Patterns must be an array' unless patterns.is_a?(Array)
                @exclude_fp = patterns
            end

            if opts.has_key?(:exclude_dir_patterns)
                patterns = opts[:exclude_dir_patterns]
                raise TypeError, 'Patterns must be an array' unless patterns.is_a?(Array)
                @exclude_dp = patterns
            end

            if opts.has_key?(:only_file_patterns)
                patterns = opts[:only_file_patterns]
                raise TypeError, 'Patterns must be an array' unless patterns.is_a?(Array)
                @only_fp = patterns
            end

            puts @only_fp.size

            root = @dir
            if opts.has_key?(:root)
                root = opts[:root]
            else
                raise "root: #{@dir}"
            end

            return int_search(root, pattern)
        end
    end
end
