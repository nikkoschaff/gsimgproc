# encoding: utf-8


require 'rubygems'
require 'bundler'
begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end
require 'rake'

require 'jeweler'
Jeweler::Tasks.new do |gem|
  # gem is a Gem::Specification... see http://docs.rubygems.org/read/chapter/20 for more options
  gem.name = "gsimgproc"
  gem.homepage = "http://github.com/nikkoschaff/gsimgproc"
  gem.license = "Copyright Gradesnap, LLC"
  gem.summary = %Q{Gradesnap Image processing}
  gem.description = %Q{Gradesnap Image processing}
  gem.email = "nikko.schaff@gmail.com"
  gem.authors = ["Nikko Schaff"]
  # dependencies defined in Gemfile
end
Jeweler::RubygemsDotOrgTasks.new

require 'rake/testtask'
Rake::TestTask.new(:test) do |test|
  test.libs << 'lib' << 'test'
  test.pattern = 'test/**/test_*.rb'
  test.verbose = true
end

=begin
require 'rcov/rcovtask'
Rcov::RcovTask.new do |test|
  test.libs << 'test'
  test.pattern = 'test/**/test_*.rb'
  test.verbose = true
  test.rcov_opts << '--exclude "gems/*"'
end
=end
task :default => :test

require 'rdoc/task'
Rake::RDocTask.new do |rdoc|
  version = File.exist?('VERSION') ? File.read('VERSION') : ""

  rdoc.rdoc_dir = 'rdoc'
  rdoc.title = "gsimgproc #{version}"
  rdoc.rdoc_files.include('README*')
  rdoc.rdoc_files.include('lib/**/*.rb')
end

                                                                     
namespace :version do
  desc "create a new version, create tag and push to github"
  task :github_and_tag do
    Rake::Task['git:release'].invoke
  end

  task :patch_release do
    Rake::Task['version:bump:patch'].invoke
    Rake::Task['version:github_and_tag'].invoke
  end

  task :minor_release do
    Rake::Task['version:bump:minor'].invoke
    Rake::Task['version:github_and_tag'].invoke
  end

  task :major_release do
    Rake::Task['version:bump:major'].invoke
    Rake::Task['version:github_and_tag'].invoke
  end
end


